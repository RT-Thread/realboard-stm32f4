#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/container.h>
#include "demo_view.h"

/*
 * ֱ����DC�ϻ�ͼ��ʵ�ֶ���Ч��
 *
 * �����������ڶ�ʱ�������ģ������·�����ʾ����
 * "�����ҷ�"
 */
static rt_int8_t dx = 1, dy = 1;
static rtgui_rect_t text_rect;
static rtgui_timer_t *timer;

void timeout(struct rtgui_timer *timer, void *parameter)
{
    struct rtgui_dc *dc;
    struct rtgui_rect rect;
    struct rtgui_widget *widget;

    /* �ؼ�(container)ͨ��parameter�������ݸ���ʱ�� */
    widget = RTGUI_WIDGET(parameter);

    /* ��ÿؼ�������DC */
    dc = rtgui_dc_begin_drawing(widget);
    if (dc == RT_NULL) /* ��������������DC�����أ�����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ��ģ� */
        return ;

    /* ���demo container�����ͼ��������Ҫ�����жϱ߽� */
    demo_view_get_rect(RTGUI_CONTAINER(widget), &rect);
    rect.y2 -= 5;

    /* �ж��Ƿ��ǵ�һ�λ�ͼ */
    if ((text_rect.x1 == 0) && (text_rect.y1 == 0))
    {
        rtgui_rect_moveto(&text_rect, rect.x1, rect.y1);
    }
    else
    {
        /* �����ϵ����� */
        rtgui_dc_fill_rect(dc, &text_rect);
    }

    /* ����dx��dy */
    if (text_rect.x2 >= rect.x2) dx = -1;
    if (text_rect.x1 < rect.x1)  dx = 1;
    if (text_rect.y2 >= rect.y2) dy = -1;
    if (text_rect.y1 < rect.y1) dy = 1;

    /* �ƶ��ı����λ�� */
    text_rect.x1 += dx;
    text_rect.x2 += dx;
    text_rect.y1 += dy;
    text_rect.y2 += dy;

    /* ��ͼ */
    rtgui_dc_draw_text(dc, "�����ҷ�", &text_rect);

    /* ��ͼ��� */
    rtgui_dc_end_drawing(dc);
}

static rt_bool_t animation_on_show(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_kprintf("animation on show\n");
    if (timer != RT_NULL)
        rtgui_timer_start(timer);
    return RT_TRUE;
}

static rt_bool_t animation_on_hide(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_kprintf("animation on hide\n");
    if (timer != RT_NULL)
        rtgui_timer_stop(timer);
    return RT_TRUE;
}

rt_bool_t animation_event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);

    if (event->type == RTGUI_EVENT_PAINT)
    {
        struct rtgui_dc *dc;
        rtgui_rect_t rect;

        /* ��Ϊ�õ���demo container�����汾����һ���ֿؼ��������ڻ�ͼʱ��Ҫ��demo container�Ȼ�ͼ */
        rtgui_container_event_handler(object, event);

        /* ��ÿؼ�������DC */
        dc = rtgui_dc_begin_drawing(widget);
        if (dc == RT_NULL) /* ��������������DC�����أ�����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ��ģ� */
            return RT_FALSE;

        /* ���demo container�����ͼ������ */
        demo_view_get_rect(RTGUI_CONTAINER(widget), &rect);

        /* �������� */
        rtgui_dc_fill_rect(dc, &rect);

        /* ��ͼ */
        rtgui_dc_draw_text(dc, "�����ҷ�", &text_rect);

        /* ��ͼ��� */
        rtgui_dc_end_drawing(dc);
    }
    else if (event->type == RTGUI_EVENT_SHOW)
    {
        rtgui_container_event_handler(object, event);
        animation_on_show(object, event);
    }
    else if (event->type == RTGUI_EVENT_HIDE)
    {
        rtgui_container_event_handler(object, event);
        animation_on_hide(object, event);
    }
    else
    {
        /* ����Ĭ�ϵ��¼������� */
        return rtgui_container_event_handler(object, event);
    }

    return RT_FALSE;
}

rtgui_container_t *demo_view_animation()
{
    rtgui_container_t *container;

    container = demo_view("DC ����");
    if (container != RT_NULL)
        rtgui_object_set_event_handler(RTGUI_OBJECT(container), animation_event_handler);

    rtgui_font_get_metrics(RTGUI_WIDGET_FONT(container), "�����ҷ�", &text_rect);
    rtgui_rect_moveto(&text_rect, 0, 45);
    timer = rtgui_timer_create(2, RT_TIMER_FLAG_PERIODIC, timeout, (void *)container);

    return container;
}
