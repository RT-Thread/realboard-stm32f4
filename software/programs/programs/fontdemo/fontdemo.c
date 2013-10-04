#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>


struct rtgui_hz_file_font hz24 =
{
    {RT_NULL},              /* cache root       */
    0,                      /* cache size       */
    24,                     /* font size        */
    72,                     /* font data size   */
    -1,                     /* fd               */
    "/SD/resource/hzk24.fnt"    /* font_fn          */
};

struct rtgui_font rtgui_font_hz24 =
{
    "hz",                   /* family */
    24,                     /* height */
    1,                      /* refer count */
    &rtgui_hz_file_font_engine,/* font engine */
    (void *) &hz24,         /* font private data */
};


/*rtgui_dc绘制基本图形函数*/
static void dc_draw(struct rtgui_widget *widget);
/*主窗口事件函数*/
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event);


/**
  * @函数名称：event_handler()
  * @描述    ：  主窗口事件服务函数
  * @参数    ：  *object: 事件对象指针,*event:事件类型指针
  * @返回值  ： rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    /*将object转换成widget*/
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
    /*输出调试信息*/
    rt_kprintf("event_handler\n");

    if (event->type == RTGUI_EVENT_PAINT)        //事件类型为RTGUI_EVENT_PAINT
                     {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_PAINT\n");
        /*执行窗体事件*/
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*调用绘制函数绘制图形*/
        dc_draw(widget);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //事件类型为RTGUI_EVENT_SHOW
               {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_SHOW\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);

    }
    else if (event->type == RTGUI_EVENT_HIDE)  //事件类型为RTGUI_EVENT_HIDE
    {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_HIDE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //事件类型为RTGUI_EVENT_WIN_DEACTIVATE
    {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //事件类型为RTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else                                       //不在范围内的事件
    {
        rt_kprintf("event->type:%d\n", event->type);
        return rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    return RT_FALSE;
}

/**
  * @函数名称：  dc_draw()
  * @描述    ：  dc绘图函数，本例子的核心
  * @参数    ：  *widget RTGUi部件指针
  * @返回值  ：  None.
  */
static void dc_draw(struct rtgui_widget *widget)
{
    /*局部变量*/
    struct rtgui_dc *dc;
    rtgui_color_t tcolor ;
    rtgui_rect_t rect = {30, 80, 320, 110};
    /*获得窗体的dc*/
    dc = rtgui_dc_begin_drawing(widget);
    /*存放当前DC的前景色*/
    tcolor = RTGUI_DC_FC(dc);
    /*设置前景色*/
    RTGUI_DC_FC(dc) = RTGUI_RGB(0, 0, 200);
    /*设置使用的字体为asc16*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("asc", 16);
    /*绘制演示字体*/
    rtgui_dc_draw_text(dc, "ASC16:hello world!", &rect);
    /*设置使用的字体为16x16汉字*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("hz", 16);
    /*设置绘制的区域*/
    rect = (rtgui_rect_t)
    {
        30, 130, 320, 160
    };
    /*绘制16X16演示字体*/
    rtgui_dc_draw_text(dc, "HZK16:你好，这是16x16字体!", &rect);
    /*设置使用字体为24x24字体*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("hz", 24);
    /*设置绘制的区域*/
    rect = (rtgui_rect_t)
    {
        30, 180, 320, 210
    };
    /*绘制24x24演示字体*/
    rtgui_dc_draw_text(dc, "HZK24:你好,这是24x24字体!", &rect);
    /*设置绘制的区域*/
    rect = (rtgui_rect_t)
    {
        30, 10, 320, 40
    };
    /*绘制标题*/
    rtgui_dc_draw_text(dc, "这是一个字库使用例程", &rect);
    /*将原来的前景色设置回去*/
    RTGUI_DC_FC(dc) = tcolor;
    /*结束dc绘图*/
    rtgui_dc_end_drawing(dc);

}

void main(void)
{
    /*局部变量*/
    struct rtgui_app *application;
    rtgui_win_t *win;
    /*建立应用程序*/
    application = rtgui_app_create("fontdemo");
    /*判断是否成功建立应用程序*/
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"fontdemo\" failed!\n");
        return ;
    }
    /*建立主窗体*/
    win = rtgui_mainwin_create(RT_NULL, "fontdemo",
                               RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    /*判断是否成功建立主窗体*/
    if (RT_NULL == win)
    {
        rt_kprintf("create main window failed!\n");
        rtgui_app_destroy(application);
        return;
    }
    /*加载外部字体*/
    rtgui_font_system_add_font(&rtgui_font_hz24);

    /*给窗体绑定事件*/
    rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*显示主窗体*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
