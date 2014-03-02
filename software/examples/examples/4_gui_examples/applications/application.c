/*
   此demo用于演示rtgui examples，系统中文字库等文件需要放在根目录
   如果触摸不准，可以删除tf卡中的setup.ini文件后重新进行触摸校准（finsh下运行calibration()）
 */
#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#endif

#include "components.h"

#include <rtgui/calibration.h>
#include "setup.h"

extern void application_init(void);
extern rt_bool_t cali_setup(void);
extern void cali_store(struct calibration_data *data);

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

	rt_platform_init();
	
    /* mount SPI flash as root directory */
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

			#if (RT_DFS_ELM_USE_LFN != 0) && (defined RT_DFS_ELM_CODE_PAGE_FILE)
            {
                extern void ff_convert_init(void);
                ff_convert_init();
            }
			#endif
        }
        else
        {
            rt_kprintf("flash0 mount to / failed!\n");
        }
    }
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
	{
		rt_device_t device;
		struct rt_device_rect_info info;
		extern void application_init(void);
		
		device = rt_device_find("lcd");
		/* re-set graphic device */
		rtgui_graphic_set_device(device);
		
		/* 在rt_components_init时由于文件系统尚未挂载，
		中文字库还没准备好，所以待中文字库准备好后，再来一次rtgui初始化*/
		#ifdef RT_USING_COMPONENTS_INIT
		rtgui_system_server_init();
		#endif
		
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
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);//
	
    if (tid != RT_NULL)
        rt_thread_startup(tid);
	
    return 0;
}


