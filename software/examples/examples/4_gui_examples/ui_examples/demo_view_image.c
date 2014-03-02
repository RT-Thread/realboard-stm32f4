/*
 * �����嵥��DC����ʾͼ����ʾ
 *
 * ������ӻ��ڴ�������container����ʾͼ��
 */

#include "demo_view.h"
#include <rtgui/widgets/button.h>
#include <rtgui/widgets/filelist_view.h>
#include <string.h>

static rtgui_image_t *image = RT_NULL;
static rtgui_container_t *_container = RT_NULL;

#if defined(RTGUI_USING_DFS_FILERW) || defined(RTGUI_USING_STDIO_FILERW)
/* �򿪰�ť�Ļص����� */
static void open_btn_onbutton(rtgui_widget_t *widget, struct rtgui_event *event)
{
    rtgui_filelist_view_t *filelist;
    struct rtgui_rect rect = {0, 100, 240, 280};

    /* WIN32ƽ̨�Ϻ���ʵ�豸�ϵĳ�ʼ·������ */
#ifdef _WIN32
    filelist = rtgui_filelist_view_create("e:\\", "*.*", &rect);
#else
    filelist = rtgui_filelist_view_create("/", "*.*", &rect);
#endif
    /* ģ̬��ʾһ���ļ��б���ͼ�����ṩ���û�ѡ��ͼ���ļ� */
    if (rtgui_widget_show(RTGUI_WIDGET(filelist), RT_TRUE) == RTGUI_MODAL_OK)
    {
        char path[32], image_type[8];

        /* �����ļ�·���ı�ǩ */
        rtgui_filelist_view_get_fullpath(filelist, path, sizeof(path));
        if (image != RT_NULL)
        {
            rtgui_image_destroy(image);
            image = RT_NULL;
        }

        rt_memset(image_type, 0, sizeof(image_type));

        /* ���ͼ������� */
        if (rt_strstr(path, ".bmp") != RT_NULL ||
                rt_strstr(path, ".BMP") != RT_NULL)
            strcat(image_type, "bmp");
        if (rt_strstr(path, ".png") != RT_NULL ||
                rt_strstr(path, ".PNG") != RT_NULL)
            strcat(image_type, "png");
        if (rt_strstr(path, ".jpg") != RT_NULL ||
                rt_strstr(path, ".JPG") != RT_NULL)
            strcat(image_type, "jpeg");
        if (rt_strstr(path, ".hdc") != RT_NULL ||
                rt_strstr(path, ".HDC") != RT_NULL)
            strcat(image_type, "hdc");

        /* ���ͼ���ļ���Ч��������Ӧ��rtgui_image���� */
        if (image_type[0] != '\0')
            image = rtgui_image_create_from_file(image_type, path, RT_TRUE);
    }

    /* ɾ�� �ļ��б� ��ͼ */
    rtgui_container_destroy(RTGUI_CONTAINER(filelist));
    rtgui_container_show(_container, RT_FALSE);
}

/* ��ʾ��ͼ���¼������� */
static rt_bool_t demo_view_event_handler(rtgui_widget_t *widget, rtgui_event_t *event)
{
    rt_bool_t result;

    /* �ȵ���Ĭ�ϵ��¼�������(����ֻ����PAINT�¼�������ʾ��ͼ���б����һЩ�ؼ�) */
    result = rtgui_container_event_handler(widget, event);

    if (event->type == RTGUI_EVENT_PAINT)
    {
        struct rtgui_dc *dc;
        rtgui_rect_t rect;

        /* ��ÿؼ�������DC */
        dc = rtgui_dc_begin_drawing(widget);
        if (dc == RT_NULL)
            /* ��������������DC������(����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ���) */
            return RT_FALSE;

        /* ���demo container�����ͼ������ */
        demo_view_get_rect(RTGUI_CONTAINER(widget), &rect);

        /* ���ͼ����ʾ���� */
        rect.x1 += 5;
        rect.x2 -= 5;
        rect.y1 += 30;

        if (image != RT_NULL)
            rtgui_image_blit(image, dc, &rect);

        /* ��ͼ��� */
        rtgui_dc_end_drawing(dc);
    }

    return result;
}

/* ����������ʾͼ�����ʾ��ͼ */
rtgui_container_t *demo_view_image(void)
{
    rtgui_rect_t rect;
    rtgui_button_t *open_btn;

    /* �ȴ���һ����ʾ��ͼ */
    _container = demo_view("ͼ����ʾ");
    if (_container != RT_NULL)
        /* ����Ĭ�ϵ��¼���������demo_view_event_handler���� */
        rtgui_object_set_event_handler(RTGUI_WIDGET(_container), demo_view_event_handler);

    /* ���һ����ť */
    demo_view_get_rect(_container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 120;
    rect.y2 = rect.y1 + 20;
    open_btn = rtgui_button_create("��ͼ���ļ�");
    rtgui_container_add_child(RTGUI_CONTAINER(_container), RTGUI_WIDGET(open_btn));
    rtgui_widget_set_rect(RTGUI_WIDGET(open_btn), &rect);
    rtgui_button_set_onbutton(open_btn, open_btn_onbutton);

    return _container;
}
#endif
