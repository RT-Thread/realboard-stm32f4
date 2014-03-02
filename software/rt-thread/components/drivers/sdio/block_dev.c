/*
 * File      : block_dev.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
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
 * Date           Author        Notes
 * 2011-07-25     weety     first version
 */

#include <rtthread.h>
#include <dfs_fs.h>

#include <drivers/mmcsd_core.h>

static rt_list_t blk_devices;

struct mmcsd_blk_device
{
    struct rt_mmcsd_card *card;
    rt_list_t list;
    struct rt_device dev;
    struct dfs_partition part;
    struct rt_device_blk_geometry geometry;
};

#ifndef RT_MMCSD_MAX_PARTITION
#define RT_MMCSD_MAX_PARTITION 16
#endif

static rt_int32_t mmcsd_num_wr_blocks(struct rt_mmcsd_card *card)
{
    rt_int32_t err;
    rt_uint32_t blocks;

    struct rt_mmcsd_req req;
    struct rt_mmcsd_cmd cmd;
    struct rt_mmcsd_data data;
    rt_uint32_t timeout_us;

    rt_memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = APP_CMD;
    cmd.arg = card->rca << 16;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_AC;

    err = mmcsd_send_cmd(card->host, &cmd, 0);
    if (err)
        return -RT_ERROR;
    if (!controller_is_spi(card->host) && !(cmd.resp[0] & R1_APP_CMD))
        return -RT_ERROR;

    rt_memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = SD_APP_SEND_NUM_WR_BLKS;
    cmd.arg = 0;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_ADTC;

    rt_memset(&data, 0, sizeof(struct rt_mmcsd_data));

    data.timeout_ns = card->tacc_ns * 100;
    data.timeout_clks = card->tacc_clks * 100;

    timeout_us = data.timeout_ns / 1000;
    timeout_us += data.timeout_clks * 1000 /
        (card->host->io_cfg.clock / 1000);

    if (timeout_us > 100000) 
    {
        data.timeout_ns = 100000000;
        data.timeout_clks = 0;
    }

    data.blksize = 4;
    data.blks = 1;
    data.flags = DATA_DIR_READ;
    data.buf = &blocks;

    rt_memset(&req, 0, sizeof(struct rt_mmcsd_req));

    req.cmd = &cmd;
    req.data = &data;

    mmcsd_send_request(card->host, &req);

    if (cmd.err || data.err)
        return -RT_ERROR;

    return blocks;
}

static rt_err_t rt_mmcsd_req_blk(struct rt_mmcsd_card *card,
                                 rt_uint32_t           sector,
                                 void                 *buf,
                                 rt_size_t             blks,
                                 rt_uint8_t            dir)
{
    void *aligned_buf;
    
    struct rt_mmcsd_cmd  cmd, stop;
    struct rt_mmcsd_data  data;
    struct rt_mmcsd_req  req;
    struct rt_mmcsd_host *host = card->host;
    rt_uint32_t r_cmd, w_cmd;

    mmcsd_host_lock(host);
    rt_memset(&req, 0, sizeof(struct rt_mmcsd_req));
    rt_memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));
    rt_memset(&stop, 0, sizeof(struct rt_mmcsd_cmd));
    rt_memset(&data, 0, sizeof(struct rt_mmcsd_data));
    req.cmd = &cmd;
    req.data = &data;
    
    cmd.arg = sector;
    if (!(card->flags & CARD_FLAG_SDHC)) 
    {
        cmd.arg <<= 9;
    }
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_ADTC;

    data.blksize = SECTOR_SIZE;
    data.blks  = blks;

    if (blks > 1) 
    {
        if (!controller_is_spi(card->host) || !dir)
        {
            req.stop = &stop;
            stop.cmd_code = STOP_TRANSMISSION;
            stop.arg = 0;
            stop.flags = RESP_SPI_R1B | RESP_R1B | CMD_AC;
        }
        r_cmd = READ_MULTIPLE_BLOCK;
        w_cmd = WRITE_MULTIPLE_BLOCK;
    }
    else
    {
        req.stop = NULL;
        r_cmd = READ_SINGLE_BLOCK;
        w_cmd = WRITE_BLOCK;
    }

    if (!dir) 
    {
        cmd.cmd_code = r_cmd;
        data.flags |= DATA_DIR_READ;
    }
    else
    {
        cmd.cmd_code = w_cmd;
        data.flags |= DATA_DIR_WRITE;
    }

    mmcsd_set_data_timeout(&data, card);
    data.buf = buf;
    mmcsd_send_request(host, &req);

    if (!controller_is_spi(card->host) && dir != 0) 
    {
        do 
        {
            rt_int32_t err;

            cmd.cmd_code = SEND_STATUS;
            cmd.arg = card->rca << 16;
            cmd.flags = RESP_R1 | CMD_AC;
            err = mmcsd_send_cmd(card->host, &cmd, 5);
            if (err) 
            {
                rt_kprintf("error %d requesting status\n", err);
                break;
            }
            /*
             * Some cards mishandle the status bits,
             * so make sure to check both the busy
             * indication and the card state.
             */
         } while (!(cmd.resp[0] & R1_READY_FOR_DATA) ||
            (R1_CURRENT_STATE(cmd.resp[0]) == 7));
    }

    mmcsd_host_unlock(host);

    if (cmd.err || data.err || stop.err) 
    {
        rt_kprintf("mmcsd request blocks error\n");
        rt_kprintf("%d,%d,%d, 0x%08x,0x%08x\n",
                   cmd.err, data.err, stop.err, data.flags, sector);

        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t rt_mmcsd_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_mmcsd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t rt_mmcsd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_mmcsd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_BLK_GETGEOME:
        rt_memcpy(args, &blk_dev->geometry, sizeof(struct rt_device_blk_geometry));
        break;
    default:
        break;
    }
    return RT_EOK;
}

static rt_size_t rt_mmcsd_read(rt_device_t dev,
                               rt_off_t    pos,
                               void       *buffer,
                               rt_size_t   size)
{
    rt_err_t err;
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    struct dfs_partition *part = &blk_dev->part;

    if (dev == RT_NULL)
    {
        rt_set_errno(-DFS_STATUS_EINVAL);

        return 0;
    }

    rt_sem_take(part->lock, RT_WAITING_FOREVER);
    err = rt_mmcsd_req_blk(blk_dev->card, part->offset + pos, buffer, size, 0);
    rt_sem_release(part->lock);

    /* the length of reading must align to SECTOR SIZE */
    if (err) 
    {
        rt_set_errno(-DFS_STATUS_EIO);
        return 0;
    }
    return size;
}

static rt_size_t rt_mmcsd_write(rt_device_t dev,
                                rt_off_t    pos,
                                const void *buffer,
                                rt_size_t   size)
{
    rt_err_t err;
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    struct dfs_partition *part = &blk_dev->part;

    if (dev == RT_NULL)
    {
        rt_set_errno(-DFS_STATUS_EINVAL);

        return 0;
    }

    rt_sem_take(part->lock, RT_WAITING_FOREVER);
    err = rt_mmcsd_req_blk(blk_dev->card, part->offset + pos, (void *)buffer, size, 1);
    rt_sem_release(part->lock);

    /* the length of reading must align to SECTOR SIZE */
    if (err) 
    {
        rt_set_errno(-DFS_STATUS_EIO);

        return 0;
    }
    return size;
}

static rt_int32_t mmcsd_set_blksize(struct rt_mmcsd_card *card)
{
    struct rt_mmcsd_cmd cmd;
    int err;

    /* Block-addressed cards ignore MMC_SET_BLOCKLEN. */
    if (card->flags & CARD_FLAG_SDHC)
        return 0;

    mmcsd_host_lock(card->host);
    cmd.cmd_code = SET_BLOCKLEN;
    cmd.arg = 512;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_AC;
    err = mmcsd_send_cmd(card->host, &cmd, 5);
    mmcsd_host_unlock(card->host);

    if (err) 
    {
        rt_kprintf("MMCSD: unable to set block size to %d: %d\n", cmd.arg, err);

        return -RT_ERROR;
    }

    return 0;
}

rt_int32_t rt_mmcsd_blk_probe(struct rt_mmcsd_card *card)
{
    rt_int32_t err = 0;
    rt_uint8_t i, status;
    rt_uint8_t *sector;
    char dname[4];
    char sname[8];
    struct mmcsd_blk_device *blk_dev = RT_NULL;

    err = mmcsd_set_blksize(card);
    if(err) 
    {
        return err;
    }

    /* get the first sector to read partition table */
    sector = (rt_uint8_t *)rt_malloc(SECTOR_SIZE);
    if (sector == RT_NULL)
    {
        rt_kprintf("allocate partition sector buffer failed\n");

        return -RT_ENOMEM;
    }

    status = rt_mmcsd_req_blk(card, 0, sector, 1, 0);
    if (status == RT_EOK)
    {
        for (i = 0; i < RT_MMCSD_MAX_PARTITION; i++)
        {
            blk_dev = rt_malloc(sizeof(struct mmcsd_blk_device));
            if (!blk_dev) 
            {
                rt_kprintf("mmcsd:malloc mem failde\n");
                break;
            }
            rt_memset((void *)blk_dev, 0, sizeof(struct mmcsd_blk_device));
            /* get the first partition */
            status = dfs_filesystem_get_partition(&blk_dev->part, sector, i);
            if (status == RT_EOK)
            {
                rt_snprintf(dname, 4, "sd%d",  i);
                rt_snprintf(sname, 8, "sem_sd%d",  i);
                blk_dev->part.lock = rt_sem_create(sname, 1, RT_IPC_FLAG_FIFO);
    
                /* register mmcsd device */
                blk_dev->dev.type = RT_Device_Class_Block;                  
                blk_dev->dev.init = rt_mmcsd_init;
                blk_dev->dev.open = rt_mmcsd_open;
                blk_dev->dev.close = rt_mmcsd_close;
                blk_dev->dev.read = rt_mmcsd_read;
                blk_dev->dev.write = rt_mmcsd_write;
                blk_dev->dev.control = rt_mmcsd_control;
                blk_dev->dev.user_data = blk_dev;

                blk_dev->card = card;
                
                blk_dev->geometry.bytes_per_sector = 1<<9;
                blk_dev->geometry.block_size = card->card_blksize;
                blk_dev->geometry.sector_count = blk_dev->part.size;
    
                rt_device_register(&blk_dev->dev, dname,
                    RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);
                rt_list_insert_after(&blk_devices, &blk_dev->list);
            }
            else
            {
                if (i == 0)
                {
                    /* there is no partition table */
                    blk_dev->part.offset = 0;
                    blk_dev->part.size   = 0;
                    blk_dev->part.lock = rt_sem_create("sem_sd0", 1, RT_IPC_FLAG_FIFO);
    
                    /* register mmcsd device */
                    blk_dev->dev.type  = RT_Device_Class_Block;
                    blk_dev->dev.init = rt_mmcsd_init;
                    blk_dev->dev.open = rt_mmcsd_open;
                    blk_dev->dev.close = rt_mmcsd_close;
                    blk_dev->dev.read = rt_mmcsd_read;
                    blk_dev->dev.write = rt_mmcsd_write;
                    blk_dev->dev.control = rt_mmcsd_control;
                    blk_dev->dev.user_data = blk_dev;

                    blk_dev->card = card;

                    blk_dev->geometry.bytes_per_sector = 1<<9;
                    blk_dev->geometry.block_size = card->card_blksize;
                    if (card->flags & CARD_FLAG_SDHC) 
                    {
                        blk_dev->geometry.sector_count = (card->csd.c_size + 1) * 1024;
                    }
                    else
                    {
                        blk_dev->geometry.sector_count = 
                            card->card_capacity * 1024 / 512;
                    }
    
                    rt_device_register(&blk_dev->dev, "sd0",
                        RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_REMOVABLE | RT_DEVICE_FLAG_STANDALONE);
                    rt_list_insert_after(&blk_devices, &blk_dev->list);
    
                    break;
                }
                else
                {
                    rt_free(blk_dev);
                    blk_dev = RT_NULL;
                    break;
                }
            }
        }
    }
    else
    {
        rt_kprintf("read mmcsd first sector failed\n");
        err = -RT_ERROR;
    }
    
    /* release sector buffer */
    rt_free(sector);
    
    return err;
}

void rt_mmcsd_blk_remove(struct rt_mmcsd_card *card)
{
    rt_list_t *l;
    struct mmcsd_blk_device *blk_dev;
    
    for (l = (&blk_devices)->next; l != &blk_devices; l = l->next)
    {
        blk_dev = (struct mmcsd_blk_device *)rt_list_entry(l, struct mmcsd_blk_device, list);
        if (blk_dev->card == card) 
        {
            rt_device_unregister(&blk_dev->dev);
            rt_list_remove(&blk_dev->list);
            rt_free(blk_dev);
        }
    }
}

void rt_mmcsd_blk_init(void)
{
    rt_list_init(&blk_devices);
}
