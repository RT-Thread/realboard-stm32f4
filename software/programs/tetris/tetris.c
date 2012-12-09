#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>



void main(void)
{
    /*局部变量*/
    struct rtgui_app *application;
    /*建立应用程序*/
    application = rtgui_app_create(rt_thread_self(), "tetris");
    /*判断是否成功建立应用程序*/
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"tetris\" failed!\n");
        return ;
    }
    tetris_ui_init(application);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
