/*
 * �����嵥���Զ���ؼ���ʾ
 *
 * ������ӻ��ڴ�������container����������Զ���ؼ�
 */
#include "demo_view.h"
#include "mywidget.h"

/* ����������ʾ�Զ���ؼ�����ͼ */
rtgui_container_t *demo_view_mywidget(void)
{
    rtgui_container_t *container;
    rtgui_rect_t rect;
    rtgui_mywidget_t *mywidget;

    /* �ȴ���һ����ʾ�õ���ͼ */
    container = demo_view("MyWidget View");

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.y1 + 80;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 80;
    /* ����һ���Զ���ؼ� */
    mywidget = rtgui_mywidget_create(&rect);
    /* container��һ��container�ؼ�������add_child�����������Կؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(mywidget));

    /* �����ͼ��λ����Ϣ */
    demo_view_get_rect(container, &rect);
    rect.x1 += 25;
    rect.x2 = rect.y1 + 40;
    rect.y1 += 5 + 100;
    rect.y2 = rect.y1 + 40;
    /* ����һ���Զ���ؼ� */
    mywidget = rtgui_mywidget_create(&rect);
    /* container��һ��container�ؼ�������add_child�����������Կؼ� */
    rtgui_container_add_child(container, RTGUI_WIDGET(mywidget));

    return container;
}
