#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

/*�����ƶ��Ĳ���*/
#define STEP_SIZE 10
/*���巽��ö��*/
enum move_dir {LU, LD, RU, RD};
/*����label����ʼλ��*/
rtgui_rect_t rect = {50, 100, 90, 120};
/*�����ʼ����*/
enum move_dir label_dir = RD;
/*������ʱ��*/
rtgui_timer_t *timer;

/**
  * @�������ƣ�dir_adjust()
  * @����    ��У��label���ƶ�����
  * @����    �� *widget �ƶ���label
  * @����ֵ  �� void.
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
  * @�������ƣ�label_move()
  * @����    ��label�ƶ�����
  * @����    �� *widget �ƶ���label
  * @����ֵ  �� void.
  */
static void label_move(struct rtgui_widget *widget)
{
    /*���ݷ�������ƶ�*/
    switch (label_dir)
    {
        /*����*/
    case LU:
        rect.x1 -= STEP_SIZE;
        rect.x2 -= STEP_SIZE;
        rect.y1 -= STEP_SIZE;
        rect.y2 -= STEP_SIZE;
        break;
        /*����*/
    case LD:
        rect.x1 -= STEP_SIZE;
        rect.x2 -= STEP_SIZE;
        rect.y1 += STEP_SIZE;
        rect.y2 += STEP_SIZE;
        break;
        /*����*/
    case RU:
        rect.x1 += STEP_SIZE;
        rect.x2 += STEP_SIZE;
        rect.y1 -= STEP_SIZE;
        rect.y2 -= STEP_SIZE;
        break;
        /*����*/
    case RD:
        rect.x1 += STEP_SIZE;
        rect.x2 += STEP_SIZE;
        rect.y1 += STEP_SIZE;
        rect.y2 += STEP_SIZE;
        break;
    }
    /*���õ�ǰλ��*/
    rtgui_widget_set_rect(widget, &rect);
}


/**
  * @�������ƣ�event_handler()
  * @����    ��  �������¼�������
  * @����    ��  *object: �¼�����ָ��,*event:�¼�����ָ��
  * @����ֵ  �� rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    /*��objectת����widget*/
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
    /*���������Ϣ*/
    rt_kprintf("event_handler\n");

    if (event->type == RTGUI_EVENT_PAINT)        //�¼�����ΪRTGUI_EVENT_PAINT
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_PAINT\n");
        /*ִ�д����¼�*/
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*����timer��ʱ��*/
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //�¼�����ΪRTGUI_EVENT_SHOW
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_SHOW\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*����timer��ʱ��*/
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_HIDE)  //�¼�����ΪRTGUI_EVENT_HIDE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_HIDE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*ֹͣtimer��ʱ��*/
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //�¼�����ΪRTGUI_EVENT_WIN_DEACTIVATE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*ֹͣtimer��ʱ��*/
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //�¼�����ΪRTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    return RT_FALSE;
}
/**
  * @�������ƣ�timeout()
  * @����    ��  �������¼�������
  * @����    ��  struct rtgui_timer *timer,void *parameter
  * @����ֵ  �� void.
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
    /*�ֲ�����*/
    struct rtgui_app *application;
    rtgui_win_t *win;
    rtgui_label_t *label;

    /*����Ӧ�ó���*/
    application = rtgui_app_create("timerdemo");
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"timerdemo\" failed!\n");
        return ;
    }
    /*����������*/
    win = rtgui_mainwin_create(RT_NULL, "timerdemo",
                               RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    if (RT_NULL == win)
    {
        rt_kprintf("create main window failed!\n");
        rtgui_app_destroy(application);
        return ;		
    }
	rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*����Label*/
    label = rtgui_label_create("hello!");
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));
    /*������ʱ������ʱ��*/
    timer = rtgui_timer_create(RT_TICK_PER_SECOND / 2, RT_TIMER_FLAG_PERIODIC, timeout, (void *)label);
    /*��ʾ������*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
