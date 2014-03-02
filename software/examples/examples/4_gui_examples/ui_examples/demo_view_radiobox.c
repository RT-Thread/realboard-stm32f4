/*
 * �����嵥��radiobox�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container�����������ͬ�����radiobox�ؼ�
 */

#include "demo_view.h"
#include <rtgui/widgets/radiobox.h>

/* ������ʾ��ֱ�����radio�ı������� */
static char *radio_item_v[5] =
{
    "one",
    "two",
    "three",
    "item 1",
    "item 2"
};

/* ������ʾˮƽ�����radio�ı������� */
static char *radio_item_h[3] =
{
    "one", "two", "three"
};

/* ����������ʾradiobox�ؼ�����ͼ */
rtgui_container_t *demo_view_radiobox(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    rtgui_radiobox_t *radiobox;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("RadioBox View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 5 * 25;

    /* ����һ����ֱ������ʾ��radiobox�ؼ����ı�����radio_item_v���飬��5���� */
    radiobox = rtgui_radiobox_create("Radio Box", RTGUI_VERTICAL, radio_item_v, 5);
    /* ���õ�ǰѡ��������ǵ�0�� */
    rtgui_radiobox_set_selection(radiobox, 0);
    /* ���radiobox�ؼ�����ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(radiobox));
    /* ����radiobox�ؼ���λ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(radiobox), &rect);

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5 + 5 * 25;
    rect.y2 = rect.y1 + 60;

    /* ����һ��ˮƽ������ʾ��radiobox�ؼ����ı�����radio_item_h���飬��3���� */
    radiobox = rtgui_radiobox_create("Radio Box", RTGUI_HORIZONTAL, radio_item_h, 3);
    /* ���õ�ǰѡ��������ǵ�0�� */
    rtgui_radiobox_set_selection(radiobox, 0);
    /* ���radiobox�ؼ�����ͼ�� */
    rtgui_container_add_child(container, RTGUI_WIDGET(radiobox));
    /* ����radiobox�ؼ���λ����Ϣ */
    rtgui_widget_set_rect(RTGUI_WIDGET(radiobox), &rect);

    return container;
}
