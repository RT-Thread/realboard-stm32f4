/*
 * �����嵥��DC����ʾͼ����ʾ
 *
 * ������ӻ��ڴ�������view����ʾͼ��
 */

#include "demo_view.h"
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/filelist_view.h>
#include <string.h>

#ifdef RT_USING_MODULE
#if defined(RTGUI_USING_DFS_FILERW)
static rtgui_container_t *_view = RT_NULL;

/* �򿪰�ť�Ļص����� */
static void open_btn_onbutton(rtgui_widget_t *widget, struct rtgui_event *event)
{
    rtgui_filelist_view_t *view;
    rtgui_workbench_t *workbench;
    rtgui_rect_t rect;

    /* ��ö����workbench */
    workbench = RTGUI_WORKBENCH(rtgui_widget_get_toplevel(widget));
    rtgui_widget_get_rect(RTGUI_WIDGET(workbench), &rect);

    /* WIN32ƽ̨�Ϻ���ʵ�豸�ϵĳ�ʼ·������ */
#ifdef _WIN32
    view = rtgui_filelist_view_create(workbench, "d:\\", "*.*", &rect);
#else
    view = rtgui_filelist_view_create(workbench, "/", "*.*", &rect);
#endif
    /* ģ̬��ʾһ���ļ��б���ͼ�����ṩ���û�ѡ��ͼ���ļ� */
    if (rtgui_container_show(RTGUI_CONTAINER(view), RT_TRUE) == RTGUI_MODAL_OK)
    {
        char path[32], name[8];

        /* �����ļ�·���ı�ǩ */
        rtgui_filelist_view_get_fullpath(view, path, sizeof(path));

        rt_memset(name, 0, sizeof(name));

        /* ���Ӧ��ģ������� */
        if (rt_strstr(path, ".mo") != RT_NULL || rt_strstr(path, ".so") != RT_NULL)
        {
            rt_module_open(path);
        }
    }

    /* ɾ�� �ļ��б� ��ͼ */
    rtgui_container_destroy(RTGUI_CONTAINER(view));
    rtgui_container_show(_view, RT_FALSE);
}

/* ����������ʾӦ��ģ�����ʾ��ͼ */
rtgui_container_t *demo_view_module(rtgui_workbench_t *workbench)
{
    rtgui_rect_t rect;
    rtgui_button_t *open_btn;

    /* �ȴ���һ����ʾ��ͼ */
    _view = demo_view(workbench, "Ӧ��ģ����ʾ");

    /* ���һ����ť */
    demo_view_get_rect(_view, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 120;
    rect.y2 = rect.y1 + 20;
    open_btn = rtgui_button_create("��Ӧ��ģ��");
    rtgui_container_add_child(RTGUI_CONTAINER(_view), RTGUI_WIDGET(open_btn));
    rtgui_widget_set_rect(RTGUI_WIDGET(open_btn), &rect);
    rtgui_button_set_onbutton(open_btn, open_btn_onbutton);

    return _view;
}
#endif
#endif

