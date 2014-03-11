#include <rtthread.h>
#include <dfs_posix.h>
#include <rtgui/calibration.h>
#include "setup.h"

#define setup_fn    "/setup.ini"

static const char* kn_touch_min_x = "touch_min_x";
static const char* kn_touch_max_x = "touch_max_x";
static const char* kn_touch_min_y = "touch_min_y";
static const char* kn_touch_max_y = "touch_max_y";

static rt_uint32_t read_line(int fd, char* line, rt_uint32_t line_size)
{
        char *pos, *next;
        rt_uint32_t length;

        length = read(fd, line, line_size);
        if (length > 0)
        {
                pos = strstr(line, "\r\n");
                if (pos == RT_NULL)
                {
                        pos = strstr(line, "\n");
                        next = pos ++;
                }
                else next = pos + 2;

                if (pos != RT_NULL)
                {
                        *pos = '\0';

                        /* move back */
                        lseek(fd, -(length - (next - line)), SEEK_CUR);

                        length = pos - line;
                }
                else length = 0;
        }

        return length;
}

static void setup_load_default(void)
{
	/*
    struct setup_items setup;

    rt_kprintf("setup_load_default!\r\n");

    setup.touch_min_x = 0x7bd;
    setup.touch_max_x = 0x20;
    setup.touch_min_y = 0x53;
    setup.touch_max_y = 0x79b;

    setup_save(&setup);
	*/
}

rt_err_t setup_load(struct setup_items* setup)
{
    int fd, length;
    char line[64];

    rt_kprintf("setup_load\n");

    fd = open(setup_fn, O_RDONLY, 0);
    if (fd >= 0)
    {
        length = read_line(fd, line, sizeof(line));
        if (strcmp(line, "[config]") == 0)
        {
            char* begin;

            // touch_min_x
            length = read_line(fd, line, sizeof(line));
            if (length == 0)
            {
                close(fd);
                setup_load_default();
                return RT_EOK;
            }
            if (strncmp(line, kn_touch_min_x, sizeof(kn_touch_min_x) - 1) == 0)
            {
                begin = strchr(line, '=');
                begin++;
                setup->touch_min_x = atoi(begin);
            }

            // touch_max_x
            length = read_line(fd, line, sizeof(line));
            if (length == 0)
            {
                close(fd);
                setup_load_default();
                return RT_EOK;
            }
            if (strncmp(line, kn_touch_max_x, sizeof(kn_touch_max_x) - 1) == 0)
            {
                begin = strchr(line, '=');
                begin++;
                setup->touch_max_x = atoi(begin);
            }

            // touch_min_y
            length = read_line(fd, line, sizeof(line));
            if (length == 0)
            {
                close(fd);
                setup_load_default();
                return RT_EOK;
            }
            if (strncmp(line, kn_touch_min_y, sizeof(kn_touch_min_y) - 1) == 0)
            {
                begin = strchr(line, '=');
                begin++;
                setup->touch_min_y = atoi(begin);
            }

            // touch_max_y
            length = read_line(fd, line, sizeof(line));
            if (length == 0)
            {
                close(fd);
                setup_load_default();
                return RT_EOK;
            }
            if (strncmp(line, kn_touch_max_y, sizeof(kn_touch_max_y) - 1) == 0)
            {
                begin = strchr(line, '=');
                begin++;
                setup->touch_max_y = atoi(begin);
            }
        }
        else
        {
            close(fd);
            setup_load_default();
            return RT_EOK;
        }
    }
    else
    {    
        close(fd);
        setup_load_default();
        return -RT_ERROR;
    }

    close(fd);
    return RT_EOK;
}

rt_err_t setup_save(struct setup_items* setup)
{
    int fd, size;
    char* p_str;
    char* buf = rt_malloc(1024);

    if (buf == RT_NULL)
    {
        rt_kprintf("no memory\r\n");
        return RT_ENOMEM;
    }

    p_str = buf;

    fd = open(setup_fn, O_WRONLY | O_TRUNC, 0);
    if (fd >= 0)
    {
        size = sprintf(p_str, "[config]\r\n"); // [config] sprintf(p_str,"")
        p_str += size;

        size = sprintf(p_str, "%s=%d\r\n", kn_touch_min_x, setup->touch_min_x); //touch_min_x
        p_str += size;

        size = sprintf(p_str, "%s=%d\r\n", kn_touch_max_x, setup->touch_max_x); //touch_max_x
        p_str += size;

        size = sprintf(p_str, "%s=%d\r\n", kn_touch_min_y, setup->touch_min_y); //touch_min_y
        p_str += size;

        size = sprintf(p_str, "%s=%d\r\n", kn_touch_max_y, setup->touch_max_y); //touch_max_y
        p_str += size;
    }

    size = write(fd, buf, p_str - buf);
    if (size == (p_str - buf))
    {
        rt_kprintf("file write succeed:\r\n");
    }

    close(fd);
    rt_free(buf);

    return RT_EOK;
}

rt_bool_t cali_setup(void)
{
    struct setup_items setup;

    if(setup_load(&setup) == RT_EOK)
    {
        struct calibration_data data;
        rt_device_t device;

        data.min_x = setup.touch_min_x;
        data.max_x = setup.touch_max_x;
        data.min_y = setup.touch_min_y;
        data.max_y = setup.touch_max_y;

        device = rt_device_find("touch");
        if(device != RT_NULL)
            rt_device_control(device, RT_TOUCH_CALIBRATION_DATA, &data);
        return RT_TRUE;
    }
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    struct setup_items setup;
    setup.touch_min_x = data->min_x;
    setup.touch_max_x = data->max_x;
    setup.touch_min_y = data->min_y;
    setup.touch_max_y = data->max_y;
    setup_save(&setup);
}

