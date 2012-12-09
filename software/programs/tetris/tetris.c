#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>



void main(void)
{
    /*�ֲ�����*/
    struct rtgui_app *application;
    /*����Ӧ�ó���*/
    application = rtgui_app_create(rt_thread_self(), "tetris");
    /*�ж��Ƿ�ɹ�����Ӧ�ó���*/
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"tetris\" failed!\n");
        return ;
    }
    tetris_ui_init(application);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
