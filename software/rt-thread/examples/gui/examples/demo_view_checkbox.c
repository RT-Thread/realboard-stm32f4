/*
 * �����嵥��checkbox�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����Ӽ���checkbox�ؼ�
 */

#include "demo_view.h"
#include <rtgui/widgets/checkbox.h>

/* ����������ʾcheckbox�ؼ�����ͼ */
rtgui_container_t *demo_view_checkbox(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    rtgui_checkbox_t *checkbox;
    rtgui_font_t *font;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("CheckBox View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ����һ��checkbox�ؼ� */
    checkbox = rtgui_checkbox_create("Red", RT_TRUE);
    /* ����ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(checkbox) = red;
    /* ����checkbox��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(checkbox), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(checkbox));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��checkbox�ؼ� */
    checkbox = rtgui_checkbox_create("Blue", RT_TRUE);
    /* ����ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(checkbox) = blue;
    /* ����checkbox��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(checkbox), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(checkbox));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��checkbox�ؼ� */
    checkbox = rtgui_checkbox_create("12 font", RT_TRUE);
    /* ��������Ϊ12���� */
    font = rtgui_font_refer("asc", 12);
    RTGUI_WIDGET_FONT(checkbox) = font;
    /* ����checkbox��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(checkbox), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(checkbox));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��checkbox�ؼ� */
    checkbox = rtgui_checkbox_create("16 font", RT_TRUE);
    /* ��������Ϊ16���� */
    font = rtgui_font_refer("asc", 16);
    RTGUI_WIDGET_FONT(checkbox) = font;
    /* ����checkbox��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(checkbox), &rect);
    rtgui_container_add_child(container, RTGUI_WIDGET(checkbox));

    return container;
}
