/*
 * File      : dfs_fs.c
 * This file is part of Device File System in RT-Thread RTOS
 * COPYRIGHT (C) 2004-2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2005-02-22     Bernard      The first version.
 * 2010-06-30     Bernard      Optimize for RT-Thread RTOS
 * 2011-03-12     Bernard      fix the filesystem lookup issue.
 */

#include <dfs_fs.h>
#include <dfs_file.h>

/**
 * @addtogroup FsApi
 */
/*@{*/

/**
 * this function will register a file system instance to device file system.
 *
 * @param ops the file system instance to be registered.
 *
 * @return RT_EOK on successful, -RT_ERROR on failed.
 */
int dfs_register(const struct dfs_filesystem_operation *ops)
{
    int ret = RT_EOK;
    const struct dfs_filesystem_operation **empty = RT_NULL;
    const struct dfs_filesystem_operation **iter;

    /* lock filesystem */
    dfs_lock();
    /* check if this filesystem was already registered */
    for (iter = &filesystem_operation_table[0];
           iter < &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX]; iter ++)
    {
        /* find out an empty filesystem type entry */
        if (*iter == RT_NULL)
            (empty == RT_NULL) ? (empty = iter) : 0;
        else if (strcmp((*iter)->name, ops->name) == 0)
        {
            ret = -1;
            break;
        }
    }

    /* save the filesystem's operations */
    if ((ret == RT_EOK) && (empty != RT_NULL))
        *empty = ops;

    dfs_unlock();
    return ret;
}

/**
 * this function will return the file system mounted on specified path.
 *
 * @param path the specified path string.
 *
 * @return the found file system or NULL if no file system mounted on
 * specified path
 */
struct dfs_filesystem *dfs_filesystem_lookup(const char *path)
{
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = RT_NULL;
    rt_uint32_t fspath, prefixlen;

    prefixlen = 0;

    /* lock filesystem */
    dfs_lock();

    /* lookup it in the filesystem table */
    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        if ((iter->path == RT_NULL) || (iter->ops == RT_NULL))
            continue;

        fspath = strlen(iter->path);
        if ((fspath < prefixlen)
            || (strncmp(iter->path, path, fspath) != 0))
            continue;

        /* check next path separator */
        if (fspath > 1 && (strlen(path) > fspath) && (path[fspath] != '/'))
            continue;

        fs = iter;
        prefixlen = fspath;
    }

    dfs_unlock();

    return fs;
}

/**
 * this function will fetch the partition table on specified buffer.
 *
 * @param part the returned partition structure.
 * @param buf the buffer contains partition table.
 * @param pindex the index of partition table to fetch.
 *
 * @return RT_EOK on successful or -RT_ERROR on failed.
 */
rt_err_t dfs_filesystem_get_partition(struct dfs_partition *part,
                                      rt_uint8_t           *buf,
                                      rt_uint32_t           pindex)
{
#define DPT_ADDRESS     0x1be       /* device partition offset in Boot Sector */
#define DPT_ITEM_SIZE   16          /* partition item size */

    rt_uint8_t *dpt;
    rt_uint8_t type;

    RT_ASSERT(part != RT_NULL);
    RT_ASSERT(buf != RT_NULL);

    dpt = buf + DPT_ADDRESS + pindex * DPT_ITEM_SIZE;

    /* check if it is a valid partition table */
    if ((*dpt != 0x80) && (*dpt != 0x00))
        return -RT_ERROR;

    /* get partition type */
    type = *(dpt+4);
    if (type == 0)
        return -RT_ERROR;

    /* set partition information
     *    size is the number of 512-Byte */
    part->type = type;
    part->offset = *(dpt+8) | *(dpt+9)<<8 | *(dpt+10)<<16 | *(dpt+11)<<24;
    part->size = *(dpt+12) | *(dpt+13)<<8 | *(dpt+14)<<16 | *(dpt+15)<<24;

    rt_kprintf("found part[%d], begin: %d, size: ",
               pindex, part->offset*512);
    if ((part->size>>11) == 0)
        rt_kprintf("%d%s",part->size>>1,"KB\n");     /* KB */
    else
    {
        unsigned int part_size;
        part_size = part->size >> 11;                /* MB */
        if ((part_size>>10) == 0)
            rt_kprintf("%d.%d%s",part_size,(part->size>>1)&0x3FF,"MB\n");
        else
            rt_kprintf("%d.%d%s",part_size>>10,part_size&0x3FF,"GB\n");
    }

    return RT_EOK;
}

/**
 * this function will mount a file system on a specified path.
 *
 * @param device_name the name of device which includes a file system.
 * @param path the path to mount a file system
 * @param filesystemtype the file system type
 * @param rwflag the read/write etc. flag.
 * @param data the private data(parameter) for this file system.
 *
 * @return 0 on successful or -1 on failed.
 */
int dfs_mount(const char   *device_name,
              const char   *path,
              const char   *filesystemtype,
              unsigned long rwflag,
              const void   *data)
{
    const struct dfs_filesystem_operation **ops;
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = RT_NULL;
    char *fullpath = RT_NULL;
    rt_device_t dev_id;

    /* open specific device */
    if (device_name == RT_NULL)
    {
        /* which is a non-device filesystem mount */
        dev_id = NULL;
    }
    else if ((dev_id = rt_device_find(device_name)) == RT_NULL)
    {
        /* no this device */
        rt_set_errno(-DFS_STATUS_ENODEV);
        return -1;
    }

    /* find out the specific filesystem */
    dfs_lock();

    for (ops = &filesystem_operation_table[0];
           ops < &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX]; ops++)
        if ((ops != RT_NULL) && (strcmp((*ops)->name, filesystemtype) == 0))
            break;

    dfs_unlock();

    if (ops == &filesystem_operation_table[DFS_FILESYSTEM_TYPES_MAX])
    {
        /* can't find filesystem */
        rt_set_errno(-DFS_STATUS_ENODEV);
        return -1;
    }

    /* check if there is mount implementation */
    if ((*ops == NULL) || ((*ops)->mount == NULL))
    {
        rt_set_errno(-DFS_STATUS_ENOSYS);
        return -1;
    }

    /* make full path for special file */
    fullpath = dfs_normalize_path(RT_NULL, path);
    if (fullpath == RT_NULL) /* not an abstract path */
    {
        rt_set_errno(-DFS_STATUS_ENOTDIR);
        return -1;
    }

    /* Check if the path exists or not, raw APIs call, fixme */
    if ((strcmp(fullpath, "/") != 0) && (strcmp(fullpath, "/dev") != 0))
    {
        struct dfs_fd fd;

        if (dfs_file_open(&fd, fullpath, DFS_O_RDONLY | DFS_O_DIRECTORY) < 0)
        {
            rt_free(fullpath);
            rt_set_errno(-DFS_STATUS_ENOTDIR);

            return -1;
        }
        dfs_file_close(&fd);
    }

    /* check whether the file system mounted or not  in the filesystem table
     * if it is unmounted yet, find out an empty entry */
    dfs_lock();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        /* check if it is an empty filesystem table entry? if it is, save fs */
        if (iter->ops == RT_NULL)
            (fs == RT_NULL) ? (fs = iter) : 0;
        /* check if the PATH is mounted */
        else if (strcmp(iter->path, path) == 0)
        {
            rt_set_errno(-DFS_STATUS_EINVAL);
            goto err1;
        }
    }

    if ((fs == RT_NULL) && (iter == &filesystem_table[DFS_FILESYSTEMS_MAX]))
    {
        rt_set_errno(-DFS_STATUS_ENOSPC);
        goto err1;
    }

    /* register file system */
    fs->path   = fullpath;
    fs->ops    = *ops;
    fs->dev_id = dev_id;
    /* release filesystem_table lock */
    dfs_unlock();

    /* open device, but do not check the status of device */
    if (dev_id != RT_NULL)
    {
        if (rt_device_open(fs->dev_id,
                           RT_DEVICE_OFLAG_RDWR) != RT_EOK)
        {
            /* The underlaying device has error, clear the entry. */
            dfs_lock();
            rt_memset(fs, 0, sizeof(struct dfs_filesystem));
            goto err1;
        }
    }

    /* call mount of this filesystem */
    if ((*ops)->mount(fs, rwflag, data) < 0)
    {
        /* close device */
        if (dev_id != RT_NULL)
            rt_device_close(fs->dev_id);

        /* mount failed */
        dfs_lock();
        /* clear filesystem table entry */
        rt_memset(fs, 0, sizeof(struct dfs_filesystem));
        goto err1;
    }

    return 0;

err1:
    dfs_unlock();
    rt_free(fullpath);

    return -1;
}

/**
 * this function will unmount a file system on specified path.
 *
 * @param specialfile the specified path which mounted a file system.
 *
 * @return 0 on successful or -1 on failed.
 */
int dfs_unmount(const char *specialfile)
{
    char *fullpath;
    struct dfs_filesystem *iter;
    struct dfs_filesystem *fs = RT_NULL;

    fullpath = dfs_normalize_path(RT_NULL, specialfile);
    if (fullpath == RT_NULL)
    {
        rt_set_errno(-DFS_STATUS_ENOTDIR);

        return -1;
    }

    /* lock filesystem */
    dfs_lock();

    for (iter = &filesystem_table[0];
            iter < &filesystem_table[DFS_FILESYSTEMS_MAX]; iter++)
    {
        /* check if the PATH is mounted */
        if ((iter->path != NULL) && (strcmp(iter->path, fullpath) == 0))
        {
            fs = iter;
            break;
        }
    }

    if (fs == RT_NULL ||
        fs->ops->unmount == RT_NULL ||
        fs->ops->unmount(fs) < 0)
    {
        goto err1;
    }

    /* close device, but do not check the status of device */
    if (fs->dev_id != RT_NULL)
        rt_device_close(fs->dev_id);

    if (fs->path != RT_NULL)
        rt_free(fs->path);

    /* clear this filesystem table entry */
    rt_memset(fs, 0, sizeof(struct dfs_filesystem));

    dfs_unlock();
    rt_free(fullpath);

    return 0;

err1:
    dfs_unlock();
    rt_free(fullpath);

    return -1;
}

/**
 * make a file system on the special device
 *
 * @param fs_name the file system name
 * @param device_name the special device name
 *
 * @return 0 on successful, otherwise failed.
 */
int dfs_mkfs(const char *fs_name, const char *device_name)
{
    int index;
    rt_device_t dev_id = RT_NULL;

    /* check device name, and it should not be NULL */
    if (device_name != RT_NULL)
        dev_id = rt_device_find(device_name);

    if (dev_id == RT_NULL)
    {
        rt_set_errno(-DFS_STATUS_ENODEV);
        return -1;
    }

    /* lock file system */
    dfs_lock();
    /* find the file system operations */
    for (index = 0; index < DFS_FILESYSTEM_TYPES_MAX; index ++)
    {
        if (filesystem_operation_table[index] != RT_NULL &&
            strcmp(filesystem_operation_table[index]->name, fs_name) == 0)
            break;
    }
    dfs_unlock();

    if (index < DFS_FILESYSTEM_TYPES_MAX)
    {
        /* find file system operation */
        const struct dfs_filesystem_operation *ops = filesystem_operation_table[index];
        if (ops->mkfs == RT_NULL)
        {
            rt_set_errno(-DFS_STATUS_ENOSYS);
            return -1;
        }

        return ops->mkfs(dev_id);
    }

    rt_kprintf("Can not find the file system which named as %s.\n", fs_name);
    return -1;
}

/**
 * this function will return the information about a mounted file system.
 *
 * @param path the path which mounted file system.
 * @param buffer the buffer to save the returned information.
 *
 * @return 0 on successful, others on failed.
 */
int dfs_statfs(const char *path, struct statfs *buffer)
{
    struct dfs_filesystem *fs;

    fs = dfs_filesystem_lookup(path);
    if (fs != RT_NULL)
    {
        if (fs->ops->statfs != RT_NULL)
            return fs->ops->statfs(fs, buffer);
    }

    return -1;
}

#ifdef RT_USING_DFS_MNTTABLE
int dfs_mount_table(void)
{
	int index = 0;

	while (1)
	{
		if (mount_table[index].path == RT_NULL) break;

		if (dfs_mount(mount_table[index].device_name,
				mount_table[index].path,
				mount_table[index].filesystemtype,
				mount_table[index].rwflag,
				mount_table[index].data) != 0)
		{
			rt_kprintf("mount fs[%s] on %s failed.\n", mount_table[index].filesystemtype,
				mount_table[index].path);
			return -RT_ERROR;
		}

		index ++;
	}
	return 0;
}
INIT_ENV_EXPORT(dfs_mount_table);
#endif

#ifdef RT_USING_FINSH
#include <finsh.h>
void mkfs(const char *fs_name, const char *device_name)
{
    dfs_mkfs(fs_name, device_name);
}
FINSH_FUNCTION_EXPORT(mkfs, make a file system);

int df(const char *path)
{
    int result;
    long long cap;
    struct statfs buffer;

    result = dfs_statfs(path ? path : RT_NULL, &buffer);
    if (result != 0)
    {
        rt_kprintf("dfs_statfs failed.\n");
        return -1;
    }

    cap = buffer.f_bsize * buffer.f_bfree / 1024;
    rt_kprintf("disk free: %d KB [ %d block, %d bytes per block ]\n",
    (unsigned long)cap, buffer.f_bfree, buffer.f_bsize);
    return 0;
}
FINSH_FUNCTION_EXPORT(df, get disk free);
#endif

/* @} */
