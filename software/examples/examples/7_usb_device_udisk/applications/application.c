/*
  此demo用于演示usb 模拟u盘功能
  使用usb线连接pc和开发板的usb slaver接口，pc端会看到一个新的盘符.
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


#ifdef RT_USING_USB_DEVICE
extern void rt_hw_usbd_init(void);
extern rt_err_t rt_usb_device_init(const char* udc_name);
extern void rt_hw_ramdisk_init(void);
#endif


#ifdef RT_USING_COMPONENTS_INIT
extern void rt_components_init(void);
#endif


extern void rt_platform_init(void);

void rt_init_thread_entry(void* parameter)
{
	rt_platform_init();
	
#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

#ifdef RT_USING_USB_DEVICE
    /* init disk first */
    rt_hw_ramdisk_init();
    /* usb device controller driver initilize */
    rt_hw_usbd_init();
    rt_usb_device_init("usbd"); 
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


