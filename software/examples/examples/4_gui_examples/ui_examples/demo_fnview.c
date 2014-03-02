/*
 * �����嵥���ļ��б���ͼ��ʾ
 *
 * ������ӻ��ȴ�����һ����ʾ�õ�view�����������İ�ťʱ�ᰴ��ģʽ��ʾ����ʽ��ʾ
 * �µ��ļ��б���ͼ��
 */
#include "demo_view.h"
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/filelist_view.h>

#if defined(RTGUI_USING_DFS_FILERW) || defined(RTGUI_USING_STDIO_FILERW)
/* ������ʾѡ���ļ������ı���ǩ */
static rtgui_label_t *label;
/* �����ļ��б���ͼ�İ�ť�ص����� */
static void open_btn_onbutton(rtgui_widget_t *widget, struct rtgui_event *event)
{
    rtgui_filelist_view_t *view;
    rtgui_workbench_t *workbench;
    rtgui_rect_t rect;

    /* ��ö����workbench���� */
    workbench = RTGUI_APPLICATION(rtgui_widget_get_toplevel(widget));
    rtgui_widget_get_rect(RTGUI_WIDGET(workbench), &rect);

    /* ���Win32ƽ̨������ƽ̨���Ĳ�ͬ����ʵĿ¼λ�� */
#ifdef _WIN32
    view = rtgui_filelist_view_create(workbench, "d:\\", "*.*", &rect);
#else
    view = rtgui_filelist_view_create(workbench, "/", "*.*", &rect);
#endif

    /* ����ģʽ��ʽ��ʾ�ļ��б���ͼ */
    if (rtgui_container_show(RTGUI_CONTAINER(view), RT_TRUE) == RTGUI_MODAL_OK)
    {
        char path[32];

        /* ���ļ��б���ͼ�гɹ�ѡ���ļ�����������Ӧ��·���� */
        rtgui_filelist_view_get_fullpath(view, path, sizeof(path));

        /* �����ļ�·�����ı���ǩ */
        rtgui_label_set_text(label, path);
    }

    /* ɾ�� �ļ��б� ��ͼ */
    rtgui_container_destroy(RTGUI_CONTAINER(view));
}

/* ����������ʾ�ļ��б���ͼ����ͼ */
rtgui_container_t *demo_fn_view(rtgui_workbench_t *workbench)
{
    rtgui_rect_t rect;
    rtgui_container_t *view;
    rtgui_button_t *open_btn;
    rtgui_font_t *font;

    /* Ĭ�ϲ���12�������ʾ */
    font = rtgui_font_refer("asc", 12);

    /* ������ʾ�õ���ͼ */
    view = demo_view(workbench, "FileList View");
    /* �����ʾ��ͼ��λ����Ϣ */
    demo_view_get_rect(view, &rect);

    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ������ʾ�ļ�·���õ��ı���ǩ */
    label = rtgui_label_create("fn: ");
    rtgui_container_add_child(RTGUI_CONTAINER(view), RTGUI_WIDGET(label));
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    RTGUI_WIDGET_FONT(RTGUI_WIDGET(label)) = font;

    /* �����ʾ��ͼ��λ����Ϣ */
    demo_view_get_rect(view, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 80;
    rect.y1 += 30;
    rect.y2 = rect.y1 + 20;
    /* ������ť�Դ���һ���µ��ļ��б���ͼ */
    open_btn = rtgui_button_create("Open File");
    rtgui_container_add_child(RTGUI_CONTAINER(view), RTGUI_WIDGET(open_btn));
    rtgui_widget_set_rect(RTGUI_WIDGET(open_btn), &rect);
    RTGUI_WIDGET_FONT(RTGUI_WIDGET(open_btn)) = font;
    rtgui_button_set_onbutton(open_btn, open_btn_onbutton);

    return view;
}
#endif
