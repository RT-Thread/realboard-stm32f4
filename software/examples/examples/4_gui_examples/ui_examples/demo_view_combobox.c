/*
 * �����嵥��label�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����Ӽ�����ͬ���͵�label�ؼ�
 */
#include "demo_view.h"
#include <rtgui/widgets/combobox.h>

struct rtgui_listbox_item items[] =
{
    {"item 1", RT_NULL},
    {"item 2", RT_NULL},
    {"item 3", RT_NULL},
    {"item 4", RT_NULL},
    {"item 5", RT_NULL},
};

/* ����������ʾcombobox�ؼ�����ͼ */
rtgui_container_t *demo_view_combobox(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    rtgui_combobox_t *box;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("ComboBox View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ����һ��label�ؼ� */
    box = rtgui_combobox_create(items, sizeof(items) / sizeof(items[0]), &rect);
    /* container��һ��container�ؼ�������add_child����������box�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(box));

    return container;
}
