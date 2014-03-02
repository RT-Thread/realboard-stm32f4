/*
  此demo用于演示sqlite数据库
 */
#include <rtthread.h>

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
	
	
# ifdef RT_USING_SQLITE
		sqlite_test();
# endif
	
    /* do some thing here. */
	
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        4*2048, RT_THREAD_PRIORITY_MAX/3, 20);//
	
    if (tid != RT_NULL)
        rt_thread_startup(tid);
	
    return 0;
}


