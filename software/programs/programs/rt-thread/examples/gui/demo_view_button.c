/*
 * �����嵥��button�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����Ӽ�����ͬ���͵�button�ؼ�
 */

#include "demo_view.h"
#include <rtgui/widgets/button.h>

/* ����������ʾbutton�ؼ�����ͼ */
rtgui_container_t *demo_view_button(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    rtgui_button_t *button;
    rtgui_font_t *font;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("Button View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ����һ��button�ؼ� */
    button = rtgui_button_create("Red");
    /* ����label�ؼ���ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(button) = red;
    /* ����button��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(button));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��button�ؼ� */
    button = rtgui_button_create("Blue");
    /* ����label�ؼ���ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(button) = blue;
    /* ����button��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(button));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��button�ؼ� */
    button = rtgui_button_create("12 font");
    /* ��������Ϊ12�����asc���� */
    font = rtgui_font_refer("asc", 12);
    RTGUI_WIDGET_FONT(button) = font;
    /* ����button��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(button));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��button�ؼ� */
    button = rtgui_button_create("16 font");
    /* ��������Ϊ16�����asc���� */
    font = rtgui_font_refer("asc", 16);
    RTGUI_WIDGET_FONT(button) = font;
    /* ����button��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(button));

    return container;
}
