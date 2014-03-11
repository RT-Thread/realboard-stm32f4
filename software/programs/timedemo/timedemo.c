#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

/*定义移动的步长*/
#define STEP_SIZE 10
/*定义方向枚举*/
enum move_dir {LU, LD, RU, RD};
/*定义label的起始位置*/
rtgui_rect_t rect = {50, 100, 90, 120};
/*定义初始方向*/
enum move_dir label_dir = RD;
/*声明定时器*/
rtgui_timer_t *timer;

/**
  * @函数名称：dir_adjust()
  * @描述    ：校正label的移动方向
  * @参数    ： *widget 移动的label
  * @返回值  ： void.
  */
static void dir_adjust(struct rtgui_widget *widget)
{

    struct rtgui_widget *parent = widget->parent;
    if (widget->extent.x1 - STEP_SIZE < parent->extent.x1 && label_dir == LD)
    {
        label_dir = RD;
    }
    else if ((widget->extent.x1 - STEP_SIZE) < parent->extent.x1 && label_dir == LU)
    {
        label_dir = RU;
    }
    else if ((widget->extent.x2 + STEP_SIZE) > parent->extent.x2 && label_dir == RD)
    {
        label_dir = LD;
    }
    else if ((widget->extent.x2 + STEP_SIZE) > parent->extent.x2 && label_dir == RU)
    {
        label_dir = LU;
    }
    else if ((widget->extent.y1 - STEP_SIZE) < parent->extent.y1 && label_dir == RU)
    {
        label_dir = RD;
    }
    else if ((widget->extent.y1 - STEP_SIZE) < parent->extent.y1 && label_dir == LU)
    {
        label_dir = LD;
    }
    else if ((widget->extent.y2 + STEP_SIZE) > parent->extent.y2 && label_dir == RD)
    {
        label_dir = RU;
    }
    else if ((widget->extent.y2 + STEP_SIZE) > parent->extent.y2 && label_dir == LD)
    {
        label_dir = LU;
    }
}
/**
  * @函数名称：label_move()
  * @描述    ：label移动函数
  * @参数    ： *widget 移动的label
  * @返回值  ： void.
  */
static void label_move(struct rtgui_widget *widget)
{
    /*根据方向进行移动*/
    switch (label_dir)
    {
        /*左上*/
    case LU:
        rect.x1 -= STEP_SIZE;
        rect.x2 -= STEP_SIZE;
        rect.y1 -= STEP_SIZE;
        rect.y2 -= STEP_SIZE;
        break;
        /*左下*/
    case LD:
        rect.x1 -= STEP_SIZE;
        rect.x2 -= STEP_SIZE;
        rect.y1 += STEP_SIZE;
        rect.y2 += STEP_SIZE;
        break;
        /*右上*/
    case RU:
        rect.x1 += STEP_SIZE;
        rect.x2 += STEP_SIZE;
        rect.y1 -= STEP_SIZE;
        rect.y2 -= STEP_SIZE;
        break;
        /*右下*/
    case RD:
        rect.x1 += STEP_SIZE;
        rect.x2 += STEP_SIZE;
        rect.y1 += STEP_SIZE;
        rect.y2 += STEP_SIZE;
        break;
    }
    /*设置当前位置*/
    rtgui_widget_set_rect(widget, &rect);
}


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
        /*启动timer定时器*/
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //事件类型为RTGUI_EVENT_SHOW
    {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_SHOW\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*启动timer定时器*/
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_HIDE)  //事件类型为RTGUI_EVENT_HIDE
    {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_HIDE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*停止timer定时器*/
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //事件类型为RTGUI_EVENT_WIN_DEACTIVATE
    {
        /*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*停止timer定时器*/
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //事件类型为RTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    return RT_FALSE;
}
/**
  * @函数名称：timeout()
  * @描述    ：  主窗口事件服务函数
  * @参数    ：  struct rtgui_timer *timer,void *parameter
  * @返回值  ： void.
  */

static void timeout(struct rtgui_timer *timer, void *parameter)
{
    struct rtgui_widget *widget;
    widget = RTGUI_WIDGET(parameter);
    rt_kprintf("into timeout\r\n");
    label_move(widget);
    dir_adjust(widget);
    rtgui_widget_update(widget->parent);
}
void main(void)
{
    /*局部变量*/
    struct rtgui_app *application;
    rtgui_win_t *win;
    rtgui_label_t *label;

    /*建立应用程序*/
    application = rtgui_app_create("timerdemo");
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"timerdemo\" failed!\n");
        return ;
    }
    /*建立主窗口*/
    win = rtgui_mainwin_create(RT_NULL, "timerdemo",
                               RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    if (RT_NULL == win)
    {
        rt_kprintf("create main window failed!\n");
        rtgui_app_destroy(application);
        return ;		
    }
	rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*建立Label*/
    label = rtgui_label_create("hello!");
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));
    /*建立定时器并绑定时间*/
    timer = rtgui_timer_create(RT_TICK_PER_SECOND / 2, RT_TIMER_FLAG_PERIODIC, timeout, (void *)label);
    /*显示主窗口*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
