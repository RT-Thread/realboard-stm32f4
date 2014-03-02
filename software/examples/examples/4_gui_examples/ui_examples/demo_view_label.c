/*
 * �����嵥��label�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����Ӽ�����ͬ���͵�label�ؼ�
 */
#include "demo_view.h"
#include <rtgui/widgets/label.h>

/* ����������ʾlabel�ؼ�����ͼ */
rtgui_container_t *demo_view_label(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    rtgui_label_t *label;
    rtgui_font_t *font;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("Label View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    label = rtgui_label_create("Red Left");
    /* ����label�ؼ��ϵ��ı����뷽ʽΪ������� */
    RTGUI_WIDGET_TEXTALIGN(label) = RTGUI_ALIGN_LEFT;
    /* ����label�ؼ���ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(label) = red;
    /* ����label��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* container��һ��container�ؼ�������add_child����������label�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    label = rtgui_label_create("Blue Right");
    /* ����label�ؼ��ϵ��ı����뷽ʽΪ���Ҷ��� */
    RTGUI_WIDGET_TEXTALIGN(label) = RTGUI_ALIGN_RIGHT;
    /* ����label�ؼ���ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(label) = blue;
    /* ����label��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* container��һ��container�ؼ�������add_child����������label�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    label = rtgui_label_create("Green Center");
    /* ����label�ؼ���ǰ��ɫΪ��ɫ */
    RTGUI_WIDGET_FOREGROUND(label) = green;
    /* ����label�ؼ��ϵ��ı����뷽ʽΪ���Ҷ��� */
    RTGUI_WIDGET_TEXTALIGN(label) = RTGUI_ALIGN_CENTER_HORIZONTAL;
    /* ����label��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* container��һ��container�ؼ�������add_child����������label�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5 + 25 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    label = rtgui_label_create("12 font");
    /* ��������Ϊ12�����asc���� */
    font = rtgui_font_refer("asc", 12);
    RTGUI_WIDGET_FONT(label) = font;
    /* ����label��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* container��һ��container�ؼ�������add_child����������label�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.y1 += 5 + 25 + 25 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    label = rtgui_label_create("16 font");
    /* ��������Ϊ16�����asc���� */
    font = rtgui_font_refer("asc", 16);
    RTGUI_WIDGET_FONT(label) = font;
    /* ����label��λ�� */
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    /* container��һ��container�ؼ�������add_child����������label�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(label));

    return container;
}

