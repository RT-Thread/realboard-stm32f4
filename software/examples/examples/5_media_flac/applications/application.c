/*
  此demo用于演示flac 软解码
  程序将tf卡挂载到系统根目录,请在tf卡放入测试歌曲，然后finsh下运行flac("/xxx.flac)来播放
  需要将sdcard目录下resource文件夹放入tf卡，以支持中文文件名
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


#ifdef RT_USING_I2C
extern rt_i2c_core_init(void);
extern rt_hw_i2c_init(void);
extern rt_err_t codec_hw_init(const char * i2c_bus_device_name);
#endif

#ifdef RT_USING_COMPONENTS_INIT
extern void rt_components_init(void);
#endif


void rt_init_thread_entry(void* parameter)
{
	
#ifdef RT_USING_I2C
    rt_i2c_core_init();
    rt_hw_i2c_init();
#endif
	
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
	#endif
        }
        else
            rt_kprintf("File System initialzation failed!\n");
#endif
    }
	
	codec_hw_init("i2c1");
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


