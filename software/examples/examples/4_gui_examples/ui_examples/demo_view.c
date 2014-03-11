#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>

#include <rtgui/widgets/container.h>
#include <rtgui/widgets/notebook.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/staticline.h>
#include <rtgui/widgets/box.h>

extern struct rtgui_notebook *the_notebook;

void demo_view_next(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_notebook_set_current_by_index(the_notebook,
                                        (rtgui_notebook_get_current_index(the_notebook) + 1) %
                                        rtgui_notebook_get_count(the_notebook));
}

void demo_view_prev(struct rtgui_object *object, struct rtgui_event *event)
{
    rt_int16_t cur_idx = rtgui_notebook_get_current_index(the_notebook);

    if (cur_idx == 0)
        rtgui_notebook_set_current_by_index(the_notebook,
                                            rtgui_notebook_get_count(the_notebook) - 1);
    else
        rtgui_notebook_set_current_by_index(the_notebook,
                                            --cur_idx);
}

rtgui_container_t *demo_view(const char *title)
{
    struct rtgui_container  *container;
    struct rtgui_label      *label;
    struct rtgui_staticline *line;
    struct rtgui_button     *next_btn, *prev_btn;
    struct rtgui_rect       rect;

    container = rtgui_container_create();
    if (container == RT_NULL)
        return RT_NULL;

    rtgui_notebook_add(the_notebook, title, RTGUI_WIDGET(container));

    /* �����ͼ��λ����Ϣ(�ڼ��뵽 notebook ��ʱ��notebook ���Զ����� container
     * �Ĵ�С) */
    rtgui_widget_get_rect(RTGUI_WIDGET(container), &rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(container), &rect);
    rect.x1 += 5;
    rect.y1 += 5;
    rect.x2 = rect.x1 + rt_strlen(title)*8;
    rect.y2 = rect.y1 + 20;

    /* ���������õı�ǩ */
    label = rtgui_label_create(title);
    /* ���ñ�ǩλ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* ��ӱ�ǩ����ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));
	
	rtgui_widget_get_rect(RTGUI_WIDGET(container), &rect);
	rtgui_widget_rect_to_device(RTGUI_WIDGET(container), &rect);
    rect.y1 += 20 + 5;
    rect.y2 = rect.y1 + 2;
    /* ����һ��ˮƽ�� staticline �� */
    line = rtgui_staticline_create(RTGUI_HORIZONTAL);
    /* ���þ�̬�ߵ�λ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(line), &rect);
    /* ��Ӿ�̬�ߵ���ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(line));

    /* �����ͼ��λ����Ϣ */
    rtgui_widget_get_rect(RTGUI_WIDGET(container), &rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(container), &rect);
    rect.x2 -= 5;
    rect.y2 -= 5;
    rect.x1 = rect.x2 - 100;
    rect.y1 = rect.y2 - 25;

    /* ����"��һ��"��ť */
    next_btn = rtgui_button_create("��һ��");
    /* ����onbutton������demo_view_next���� */
    rtgui_button_set_onbutton(next_btn, demo_view_next);
    /* ���ð�ť��λ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(next_btn), &rect);
    /* ��Ӱ�ť����ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(next_btn));

    /* �����ͼ��λ����Ϣ */
    rtgui_widget_get_rect(RTGUI_WIDGET(container), &rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(container), &rect);
    rect.x1 += 5;
    rect.y2 -= 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 = rect.y2 - 25;

    /* ����"��һ��"��ť */
    prev_btn = rtgui_button_create("��һ��");
    /* ����onbutton������demo_view_prev���� */
    rtgui_button_set_onbutton(prev_btn, demo_view_prev);
    /* ���ð�ť��λ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(prev_btn), &rect);
    /* ��Ӱ�ť����ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(prev_btn));

    /* ���ش�������ͼ */
    return container;
}

/* ����������ڷ�����ʾ��ͼ�Ķ���������� */
void demo_view_get_rect(rtgui_container_t *container, rtgui_rect_t *rect)
{
    RT_ASSERT(container != RT_NULL);
    RT_ASSERT(rect != RT_NULL);

    rtgui_widget_get_rect(RTGUI_WIDGET(container), rect);
    rtgui_widget_rect_to_device(RTGUI_WIDGET(container), rect);
    /* ȥ����ʾ������·���ť������ */
    rect->y1 += 45;
    rect->y2 -= 35;
}

void demo_view_get_logic_rect(rtgui_container_t *container, rtgui_rect_t *rect)
{
    RT_ASSERT(container != RT_NULL);
    RT_ASSERT(rect != RT_NULL);

    rtgui_widget_get_rect(RTGUI_WIDGET(container), rect);
    /* ȥ����ʾ������·���ť������ */
    rect->y1 += 45;
    rect->y2 -= 35;
}
