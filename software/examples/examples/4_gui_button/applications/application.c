/*
   ��demo��һ�����׵�rtgui��ʾ������lcd������ʾhello world
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

#include "ui_button.h"


void rt_init_thread_entry(void* parameter)
{
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

    rt_platform_init();
	
	
#ifdef RT_USING_RTGUI
	{
		rt_device_t device;
		struct rt_device_rect_info info;
		extern void application_init(void);

		device = rt_device_find("lcd");
		/* re-set graphic device */
		rtgui_graphic_set_device(device);
		
		/* ��rt_components_initʱ�����ļ�ϵͳ��δ���أ�
		�����ֿ⻹û׼���ã����Դ������ֿ�׼���ú�����һ��rtgui��ʼ��*/
		#ifdef RT_USING_COMPONENTS_INIT
		rtgui_system_server_init();
		#endif
	
		/* create button example */
		ui_button();
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
