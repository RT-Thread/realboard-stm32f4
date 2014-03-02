/*
  ��demo������ʾusb cdc
  ʹ��usb������pc�Ϳ������usb slaver�ӿڣ����г����pc��ʾ�������豸��
  ��װ��usb cdc pc��������Ȼ��pc�˻�����µĴ����豸����ʱϵͳ�Ĵ��ڿ���̨�л����˴����豸��
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
extern void rt_usb_vcom_init(void);
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
    /* usb device controller driver initilize */
     rt_hw_usbd_init();

     rt_usb_device_init("usbd");
     
#ifdef RT_USB_DEVICE_CDC
    rt_usb_vcom_init();

#ifdef RT_USING_CONSOLE
    rt_console_set_device("vcom");
#endif
#ifdef RT_USING_FINSH
    finsh_set_device("vcom");
#endif
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


