/*
  此demo用于演示romfs，程序将tf卡挂载到系统根目录,romfs挂在到/romdisk 目录下
  需要先在根目录建立romdisk目录 (finsh mkdir("/romdisk"))
 */
#include <rtthread.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <dfs_romfs.h>
#endif

#include "components.h"


void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

	{
		void rt_hw_sdcard_init(void);
    	rt_hw_sdcard_init();
	}
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
        }
        else
            rt_kprintf("File System initialzation failed!\n");

		#if defined(RT_USING_DFS_ROMFS)
		if (dfs_mount(RT_NULL, "/romdisk", "rom", 0, DFS_ROMFS_ROOT) == 0)
		{
			rt_kprintf("ROM File System initialized!\n");
		}
		else
			rt_kprintf("ROM File System initialzation failed!\n");
		#endif		

	#endif
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


