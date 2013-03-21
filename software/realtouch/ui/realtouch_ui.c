#include <rtthread.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/calibration.h>
#include "setup.h"
#include "appmgr.h"
#include "statusbar.h"

rt_bool_t cali_setup(void)
{
    struct setup_items setup;

    if(setup_load(&setup) == RT_EOK)
    {
        struct calibration_data data;
        rt_device_t device;

        data.min_x = setup.touch_min_x;
        data.max_x = setup.touch_max_x;
        data.min_y = setup.touch_min_y;
        data.max_y = setup.touch_max_y;

        device = rt_device_find("touch");
        if(device != RT_NULL)
            rt_device_control(device, RT_TOUCH_CALIBRATION_DATA, &data);
        return RT_TRUE;
    }
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    struct setup_items setup;
    setup.touch_min_x = data->min_x;
    setup.touch_max_x = data->max_x;
    setup.touch_min_y = data->min_y;
    setup.touch_max_y = data->max_y;
    setup_save(&setup);
}

void realtouch_ui_init(void)
{
    rt_device_t device;    
    struct rt_device_rect_info info;    

    device = rt_device_find("lcd");    
    if (device != RT_NULL)    
    {        
        info.width = 800;        
        info.height = 480;        
        /* set graphic resolution */        
        rt_device_control(device, RTGRAPHIC_CTRL_SET_MODE, &info);    
    }    

    /* re-set graphic device */    
    rtgui_graphic_set_device(device); 
    /*font system init*/		
    rtgui_font_system_init();
    app_mgr_init();
    rt_thread_delay(10);

}

