/*
 * �����嵥��notebook�ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����ʾnotebook�ؼ�
 */

#include "demo_view.h"
#include <rtgui/widgets/notebook.h>
#include <rtgui/widgets/listbox.h>

const static struct rtgui_listbox_item items[] =
{
    {"list #0", RT_NULL},
    {"list #1", RT_NULL},
    {"list #2", RT_NULL},
    {"list #3", RT_NULL},
};

const static struct rtgui_listbox_item items2[] =
{
    {"list #0", RT_NULL},
    {"list #1", RT_NULL},
    {"list #2", RT_NULL},
    {"new list #1", RT_NULL},
    {"new list #2", RT_NULL},
};

/* ����������ʾnotebook�ؼ�����ͼ */
rtgui_container_t *demo_view_notebook(void)
{
    rtgui_rect_t rect;
    rtgui_container_t *container;
    struct rtgui_notebook *notebook;
    rtgui_listbox_t *box;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("Notebook View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);

    notebook = rtgui_notebook_create(&rect, RTGUI_NOTEBOOK_BOTTOM);
    /* container��һ��container�ؼ�������add_child����������notebook�ؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(notebook));

    box = rtgui_listbox_create(items, sizeof(items) / sizeof(struct rtgui_listbox_item), &rect);
    rtgui_notebook_add(notebook, "Tab 1", RTGUI_WIDGET(box));

    box = rtgui_listbox_create(items2, sizeof(items2) / sizeof(struct rtgui_listbox_item), &rect);
    rtgui_notebook_add(notebook, "Tab 2", RTGUI_WIDGET(box));

    return container;
}
