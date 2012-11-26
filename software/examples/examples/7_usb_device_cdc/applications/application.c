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

#include "stm32f4xx.h"
#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_COMPONENTS_INIT
#include <components.h>
#endif

#ifdef RT_USING_USB_DEVICE
extern void rt_hw_usbd_init(void);
extern rt_err_t rt_usb_device_init(const char* udc_name);
extern void rt_usb_vcom_init(void);
#endif

extern void rt_platform_init(void);

void rt_init_thread_entry(void* parameter)
{
	rt_platform_init();

#ifdef RT_USING_COMPONENTS_INIT
	/* initialization RT-Thread Components */
	rt_components_init();
#endif

#ifdef RT_USING_USB_DEVICE
    /* usb device controller driver initilize */
     rt_hw_usbd_init();

     rt_usb_device_init("usbd");
     
#ifdef RT_USB_DEVICE_CDC
    rt_usb_vcom_init();

#ifdef RT_USING_CONSOLE
    rt_console_set_device("vcom");
#endif
#ifdef RT_USING_FINSH
    finsh_set_device("vcom");
#endif
#endif
    
#endif
}

int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
