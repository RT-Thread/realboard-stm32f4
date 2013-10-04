/*
 * �����嵥��DC������ʾ
 *
 * ������ӻ��ڴ�������view�Ͻ���DC��������ʾ
 */

#include "demo_view.h"
#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/slider.h>
#include <rtgui/image_hdc.h>

#include "play.hdh"
#include "stop.hdh"
struct rtgui_image_hdcmm play_image = RTGUI_IMAGE_HDC_DEF(2, 0x1c, 0x16, play_hdh);
struct rtgui_image_hdcmm stop_image = RTGUI_IMAGE_HDC_DEF(2, 0x1c, 0x16, stop_hdh);

/*
 * view���¼�������
 */
rt_bool_t dc_event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);

    /* ����PAINT�¼����д��� */
    if (event->type == RTGUI_EVENT_PAINT)
    {
        struct rtgui_dc *dc;
        rtgui_rect_t rect;
        const int vx[] = {20, 50, 60, 45, 60, 20};
        const int vy[] = {150, 50, 90, 60, 45, 50};

        /*
         * ��Ϊ�õ���demo view�����汾����һ���ֿؼ��������ڻ�ͼʱ��Ҫ��demo view
         * �Ȼ�ͼ
         */
        rtgui_container_event_handler(RTGUI_OBJECT(widget), event);

        /************************************************************************/
        /* �������DC�Ĳ���                                                     */
        /************************************************************************/

        /* ��ÿؼ�������DC */
        dc = rtgui_dc_begin_drawing(widget);
        /* ��������������DC�����أ�����ؼ��򸸿ؼ�������״̬��DC�ǻ�ȡ���ɹ��ģ� */
        if (dc == RT_NULL)
            return RT_FALSE;

        /* ���demo view�����ͼ������ */
        demo_view_get_logic_rect(RTGUI_CONTAINER(widget), &rect);

        RTGUI_DC_TEXTALIGN(dc) = RTGUI_ALIGN_BOTTOM | RTGUI_ALIGN_CENTER_HORIZONTAL;
        /* ��ʾGUI�İ汾��Ϣ */
#ifdef RTGUI_USING_SMALL_SIZE
        rtgui_dc_draw_text(dc, "RT-Thread/GUIС�Ͱ汾", &rect);
#else
        rtgui_dc_draw_text(dc, "RT-Thread/GUI��׼�汾", &rect);
#endif

        {
            rtgui_rect_t rect = {0, 0, 0x1c, 0x16};
            rtgui_rect_moveto(&rect, 80, 80);
            rtgui_image_blit((rtgui_image_t *)&play_image, dc, &rect);

            rect.x1 = 0;
            rect.y1 = 0;
            rect.x2 = 0x1c;
            rect.y2 = 0x16;
            rtgui_rect_moveto(&rect, 130, 80);
            rtgui_image_blit((rtgui_image_t *)&stop_image, dc, &rect);
        }
        /* ����һ��Բ�� */
        RTGUI_DC_FC(dc) = red;
        rtgui_dc_draw_circle(dc, rect.x1 + 10, rect.y1 + 10, 10);

        /* ���һ��Բ�� */
        RTGUI_DC_FC(dc) = green;
        rtgui_dc_fill_circle(dc, rect.x1 + 30, rect.y1 + 10, 10);
#if 0
        /* ��һ��Բ�Ǿ��� */
        rect.x1 = 150;
        rect.y1 = 180;
        rect.x2 = 210;
        rect.y2 = 260;
        RTGUI_DC_FC(dc) = RTGUI_RGB(25, 70, 150);
        rtgui_dc_draw_round_rect(dc, &rect, 10);

        rect.x1 = 160;
        rect.y1 = 190;
        rect.x2 = 200;
        rect.y2 = 250;
        RTGUI_DC_FC(dc) = RTGUI_RGB(170, 7, 80);
        rtgui_dc_fill_round_rect(dc, &rect, 7);

        /* ��һ��Բ�� */
        RTGUI_DC_FC(dc) = RTGUI_RGB(250, 120, 120);
        rtgui_dc_draw_arc(dc, rect.x1 + 120, rect.y1 + 60, 30, 0, 120);

        /* ��һ������Բ�� */
        RTGUI_DC_FC(dc) = RTGUI_RGB(150, 23, 100);
        rtgui_dc_draw_annulus(dc, 180, 170, 30, 50, 210, 330);

        /* ����� */
        RTGUI_DC_FC(dc) = blue;
        rtgui_dc_draw_polygon(dc, vx, vy, 6);

#endif
        RTGUI_DC_FC(dc) = blue;

        /* ���Ʋ�ͬ�ı߿� */
        {
            rtgui_rect_t rect = {0, 0, 16, 16};
            rtgui_rect_moveto(&rect, 30, 120);

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_RAISE);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "raise", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_SIMPLE);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "simple", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_SUNKEN);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "sunken", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_BOX);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "box", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_STATIC);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "static", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;

            rtgui_dc_draw_border(dc, &rect, RTGUI_BORDER_EXTRA);
            rect.x1 += 20;
            rect.x2 += 20 + 50;
            rtgui_dc_draw_text(dc, "extera", &rect);
            rect.x1 -= 20;
            rect.x2 -= 20 + 50;
            rect.y1 += 20;
            rect.y2 += 20;
        }

        /* ��ͼ��� */
        rtgui_dc_end_drawing(dc);
    }
    else
    {
        /* �����¼�������Ĭ�ϵ��¼������� */
        return rtgui_container_event_handler(object, event);
    }

    return RT_FALSE;
}

/* ��������DC������ʾ�õ���ͼ */
rtgui_container_t *demo_view_dc(void)
{
    rtgui_container_t *view;

    view = demo_view("DC Demo");
    if (view != RT_NULL)
        /* ���ó��Լ����¼������� */
        rtgui_object_set_event_handler(RTGUI_OBJECT(view), dc_event_handler);

    return view;
}
