/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <stdio.h>
#include <rtthread.h>
#include <board.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#endif

#ifdef RT_USING_USB_HOST
extern void rt_hw_susb_init(void);
#endif

#ifdef RT_USING_LWIP
extern rt_hw_stm32_eth_init(void);
#endif

#ifdef RT_USING_I2C
extern rt_i2c_core_init(void);
extern rt_hw_i2c_init(void);
#endif

#ifdef RT_USING_COMPONENTS_INIT
extern rt_components_init(void);
#endif

#ifdef RT_USING_RTGUI
extern realtouch_ui_init(void);
#endif

extern rt_platform_init(void);
extern rt_err_t codec_hw_init(const char *bus_name);

void rt_init_thread_entry(void *parameter)
{
#ifdef RT_USING_LWIP
    /* initialize eth interface */
    rt_hw_stm32_eth_init();
#endif

#ifdef RT_USING_I2C
    rt_i2c_core_init();
    rt_hw_i2c_init();
#endif

#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    rt_platform_init();

    /* Filesystem Initialization */
#ifdef RT_USING_DFS
    {
        rt_bool_t mount_flag = RT_FALSE;

        if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
        {
            mount_flag = RT_TRUE;
        }
        else
        {
            rt_kprintf("flash0 mount to / failed!, fatmat and try again!\n");

            /* fatmat filesystem. */
            dfs_mkfs("elm", "flash0");

            /* re-try mount. */
            if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
            {
                mount_flag = RT_TRUE;
            }
        }

        if(mount_flag == RT_TRUE)
        {
            rt_kprintf("flash0 mount to / \n");

            /* download resource from www.rt-thread.org */
            {
                resource_download();
            }

#if (RT_DFS_ELM_USE_LFN != 0) && (defined RT_DFS_ELM_CODE_PAGE_FILE)
            {
                extern void ff_convert_init(void);
                ff_convert_init();
            }
#endif

            if (dfs_mount("sd0", "/SD", "elm", 0, 0) == 0)
            {
                rt_kprintf("sd0 mount to /SD \n");
            }
            else
            {
                rt_kprintf("sd0 mount to /SD failed!\n");
            }
        }
        else
        {
            rt_kprintf("flash0 mount to / failed!\n");
        }
    }
#endif /* RT_USING_DFS */

#ifdef RT_USING_USB_DEVICE
    /* usb device controller driver initilize */
    rt_hw_usbd_init();

    rt_usb_device_init("usbd");
#endif /* RT_USING_USB_DEVICE */

#if STM32_EXT_SRAM
    /* init netbuf worker */
    net_buf_init(320 * 1024);
#endif

    codec_hw_init("i2c1");

#ifdef RT_USING_RTGUI
    realtouch_ui_init();
#endif
}

int rt_application_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                           rt_init_thread_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX/3, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

/*@}*/
