/*
   此demo类似数码相框，请将HDC或BMP格式图片放在TF卡根目录进行显示，左右键用于切换图片
 */

#include <board.h>
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#endif

#include "components.h"


void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    rt_platform_init();
	
    /* Filesystem Initialization */
#ifdef RT_USING_DFS
    {
	#ifdef RT_USING_DFS_ELMFAT
        /* mount sd card fat partition 1 as root directory */
        if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("File System initialized!\n");

			/*  初始化中文查找表 unicode to gb2312*/
			#if (RT_DFS_ELM_USE_LFN != 0) && (defined RT_DFS_ELM_CODE_PAGE_FILE)
            {
                extern void ff_convert_init(void);
                ff_convert_init();
            }
			#endif
	#endif
        }
        else
            rt_kprintf("File System initialzation failed!\n");
#endif
    }
	
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
	
		picture_show();
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

