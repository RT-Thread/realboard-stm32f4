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

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

// #include <drivers/i2c.h>
// extern struct rt_i2c_bus_device stm32_i2c1;

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_I2C
    rt_i2c_core_init();
    rt_hw_i2c_init();
#endif

    /* initialization RT-Thread Components
	 *  finsh
	 *  filesystems, such as fatfs, yaffs, uffs
	 *  GUI
	 *  Lwip
	 */
    rt_components_init();
rt_platform_init();
    /* Filesystem Initialization */
#ifdef RT_USING_DFS
//    rt_hw_sdcard_init();

    {
        /* init the device filesystem */
        dfs_init();

#ifdef RT_USING_DFS_ELMFAT
        /* init the elm chan FatFs filesystam*/
        elm_init();

        /* mount sd card fat partition 1 as root directory */
        if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("File System initialized!\n");
        }
        else
            rt_kprintf("File System initialzation failed!\n");
#endif
    }
#endif

    /* do some thing here. */
    rt_kprintf("codec init begin...\n");
    codec_hw_init("i2c1");
    rt_kprintf("codec init over\n");

    {
        printf("printf test\r\n");
    }
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
