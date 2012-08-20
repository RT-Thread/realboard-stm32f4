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

#include "stm32f4xx.h"
#include <board.h>
#include <rtthread.h>

void rt_init_thread_entry(void *parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    rt_platform_init();

    /* Filesystem Initialization */
#ifdef RT_USING_DFS
    /* mount sd card fat partition 1 as root directory */
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else rt_kprintf("File System initialzation failed!\n");
#endif

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
