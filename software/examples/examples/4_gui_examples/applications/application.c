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
#include <rtgui/calibration.h>

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>

#ifdef RT_USING_DFS_ROMFS
#include <dfs_romfs.h>
#endif

#endif

rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
            data->min_x,
            data->max_x,
            data->min_y,
            data->max_y);
}

void rt_init_thread_entry(void* parameter)
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

#define DFS_ROMFS_ROOT (&romfs_root)

//#if defined(RT_USING_DFS_ROMFS)
//	dfs_romfs_init();
//	if (dfs_mount(RT_NULL, "/", "rom", 0, DFS_ROMFS_ROOT) == 0)
//	{
//		rt_kprintf("ROM File System initialized!\n");
//	}
//	else
//		rt_kprintf("ROM File System initialzation failed!\n");
//#endif

#ifdef RT_USING_RTGUI
	{
		rt_device_t device;
		struct rt_device_rect_info info;
		extern void application_init(void);

		device = rt_device_find("lcd");
		/* re-set graphic device */
		rtgui_graphic_set_device(device);

        calibration_set_restore(cali_setup);
        calibration_set_after(cali_store);
        calibration_init();
		application_init();
	}
#endif

    /* do some thing here. */
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
