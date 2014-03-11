/*
   此demo用于演示ftp服务器
 */
#include <board.h>
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


#include "components.h"

#ifdef RT_USING_LWIP
extern rt_hw_stm32_eth_init(void);
#endif

#ifdef RT_USING_COMPONENTS_INIT
extern void rt_components_init(void);
#endif

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_LWIP
	/* initialize eth interface */
	rt_hw_stm32_eth_init();
#endif

#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    rt_platform_init();

    /* mount SPI flash as root directory */
    if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("flash0 mount to /.\n");
		#if (RT_DFS_ELM_USE_LFN != 0) && (defined RT_DFS_ELM_CODE_PAGE_FILE)
		{
             extern void ff_convert_init(void);
             ff_convert_init();
		}
		#endif
    }
    else
    {
        rt_kprintf("flash0 mount to / failed.\n");
    }
	
    /* do some thing here. */
#if defined(RT_USING_DFS) && defined(RT_USING_LWIP)
	/* start ftp server */
	rt_kprintf("ftp server begin...\n");
	ftpd_start();
	rt_kprintf("ftp server started!!\n");
#endif
    
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

