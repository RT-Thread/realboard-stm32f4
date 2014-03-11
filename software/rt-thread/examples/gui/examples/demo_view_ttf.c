/*
 * �����嵥��TTF������ʾ��ʾ
 *
 * ������ӻ��ڴ�������container�Ͻ���TTF������ʾ����ʾ
 */

#include "demo_view.h"
#include <rtgui/dc.h>
#include <rtgui/font.h>
#include <rtgui/rtgui_system.h>

#ifdef RTGUI_USING_TTF
static rtgui_font_t *font_16, *font_24, *font_36, *font_48;

/*
 * container���¼�������
 */
rt_bool_t ttf_event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);

    /* ����PAINT�¼����д��� */
    if (event->type == RTGUI_EVENT_PAINT)
    {
        struct rtgui_dc *dc;
        rtgui_rect_t rect;
        rtgui_font_t *saved;

        /*
         * ��Ϊ�õ���demo container�����汾����һ���ֿؼ��������ڻ�ͼʱ��Ҫ��demo container
         * �Ȼ�ͼ
         */
        rtgui_container_event_handler(widget, event);

        /************************************************************************/
        /* �������DC�Ĳ���                                                     */
        /************************************************************************/

        /* ��ÿؼ�������DC */
        dc = rtgui_dc_begin_drawing(widget);
        /* ��������������DC�����أ�����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ��ģ� */
        if (dc == RT_NULL)
            return RT_FALSE;

        /* ���demo container�����ͼ������ */
        demo_view_get_rect(RTGUI_CONTAINER(widget), &rect);

        saved = RTGUI_WIDGET_FONT(widget);

        RTGUI_WIDGET_FONT(widget) = font_16;
        rtgui_dc_draw_text(dc, "ABCD����", &rect);
        rect.y1 += 18;

        RTGUI_WIDGET_FONT(widget) = font_24;
        rtgui_dc_draw_text(dc, "ABCD����", &rect);
        rect.y1 += 26;

        RTGUI_WIDGET_FONT(widget) = font_36;
        rtgui_dc_draw_text(dc, "ABCD����", &rect);
        rect.y1 += 38;

        RTGUI_WIDGET_FONT(widget) = font_48;
        rtgui_dc_draw_text(dc, "ABCD����", &rect);

        RTGUI_WIDGET_FONT(widget) = saved;
        /* ��ͼ��� */
        rtgui_dc_end_drawing(dc);
    }
    else
    {
        /* �����¼�������Ĭ�ϵ��¼������� */
        return rtgui_container_event_handler(widget, event);
    }

    return RT_FALSE;
}

/* ��������TTF������ʾ��ʾ�õ���ͼ */
rtgui_container_t *demo_view_ttf()
{
    rtgui_container_t *container;

    font_16 = rtgui_freetype_font_create("d:/simsun.ttf", 0, 0, 16);
    font_24 = rtgui_freetype_font_create("d:/simsun.ttf", 0, 0, 24);
    font_36 = rtgui_freetype_font_create("d:/simsun.ttf", 0, 0, 36);
    font_48 = rtgui_freetype_font_create("d:/simsun.TTF", 0, 0, 72);

    container = demo_view("TTF ��ʾ");
    if (container != RT_NULL)
    {
        RTGUI_WIDGET_BACKGROUND(container) = white;
        /* ���ó��Լ����¼������� */
        rtgui_object_set_event_handler(RTGUI_OBJECT(container), ttf_event_handler);
    }

    return container;
}
#endif
