/*
  此demo用于演示uffs(On nand flash)，程序将nand flash挂载到系统根目录
  需要根据情况配置uffs_config.h
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

#ifdef RT_USING_MTD_NAND
#include "k9f2g08u0b.h"
#endif    

#include "components.h"

void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

#ifdef RT_USING_MTD_NAND
    rt_hw_mtd_nand_init();

#ifdef RT_USING_DFS_UFFS
	/* mount nand flash partition 0 as root directory */
	if (dfs_mount("nand0", "/", "uffs", 0, 0) == 0)
	{
		rt_kprintf("uffs initialized!\n");
		/*  初始化中文查找表 unicode to gb2312*/
		#if (RT_DFS_ELM_USE_LFN != 0) && (defined RT_DFS_ELM_CODE_PAGE_FILE)
        {
              extern void ff_convert_init(void);
              ff_convert_init();
        }
	    #endif
	}
	else
		rt_kprintf("uffs initialzation failed!\n");
#endif

#endif    

    /* do some thing here. */
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
        rt_init_thread_entry, RT_NULL,
        2048, RT_THREAD_PRIORITY_MAX/3, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
