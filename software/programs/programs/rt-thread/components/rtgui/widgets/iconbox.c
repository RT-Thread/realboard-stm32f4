/*
 * File      : iconbox.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2009, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-10-16     Bernard      first version
 */
#include <rtgui/dc.h>
#include <rtgui/widgets/iconbox.h>
#include <rtgui/rtgui_theme.h>

static void _rtgui_iconbox_constructor(rtgui_iconbox_t *iconbox)
{
    /* init widget and set event handler */
    RTGUI_WIDGET(iconbox)->flag |= (RTGUI_WIDGET_FLAG_TRANSPARENT | RTGUI_WIDGET_FLAG_FOCUSABLE);
    rtgui_object_set_event_handler(RTGUI_OBJECT(iconbox), rtgui_iconbox_event_handler);

    /* set proper of control */
    iconbox->image = RT_NULL;
    iconbox->selected = RT_FALSE;
    iconbox->text = RT_NULL;
    iconbox->text_position = RTGUI_ICONBOX_TEXT_BELOW;
}

static void _rtgui_iconbox_destructor(rtgui_iconbox_t *iconbox)
{
    if (iconbox->image != RT_NULL)
    {
        rtgui_image_destroy(iconbox->image);
        iconbox->image = RT_NULL;
    }

    if (iconbox->text != RT_NULL)
    {
        rt_free(iconbox->text);
        iconbox->text = RT_NULL;
    }
}

DEFINE_CLASS_TYPE(iconbox, "iconbox",
                  RTGUI_WIDGET_TYPE,
                  _rtgui_iconbox_constructor,
                  _rtgui_iconbox_destructor,
                  sizeof(struct rtgui_iconbox));

rt_bool_t rtgui_iconbox_event_handler(struct rtgui_object *object, struct rtgui_event *event)
{
    struct rtgui_iconbox *iconbox;

    iconbox = RTGUI_ICONBOX(object);

    switch (event->type)
    {
    case RTGUI_EVENT_PAINT:
        rtgui_theme_draw_iconbox(iconbox);
        break;

    case RTGUI_EVENT_MOUSE_BUTTON:
        if (RTGUI_WIDGET_IS_HIDE(object)) return RT_FALSE;

        {
            struct rtgui_event_mouse *emouse = (struct rtgui_event_mouse *)event;

            /* it's not this widget event, clean status */
            if (rtgui_rect_contains_point(&(RTGUI_WIDGET(iconbox)->extent),
                                          emouse->x, emouse->y) != RT_EOK)
            {
                if (iconbox->selected != RT_TRUE)
                {
                    rtgui_iconbox_set_selected(iconbox, RT_TRUE);
                    rtgui_widget_focus(RTGUI_WIDGET(iconbox));
                }
                break;
            }
        }
        return RT_TRUE;

    default:
        return rtgui_widget_event_handler(object, event);
    }

    return RT_FALSE;
}

struct rtgui_iconbox *rtgui_iconbox_create(struct rtgui_image *image,
        const char *text,
        int position)
{
    struct rtgui_iconbox *iconbox;

    iconbox = (struct rtgui_iconbox *)rtgui_widget_create(RTGUI_ICONBOX_TYPE);
    if (iconbox != RT_NULL)
    {
        rtgui_rect_t rect = {0, 0, 0, 0}, text_rect;

        rect.x2 = image->w;
        rect.y2 = image->h;

        /* get text rect */
        rtgui_font_get_metrics(rtgui_font_default(), text, &text_rect);
        if (position == RTGUI_ICONBOX_TEXT_BELOW)
        {
            rect.y2 += RTGUI_WIDGET_DEFAULT_MARGIN;
            if (text_rect.x2 > rect.x2)
            {
                rect.x2 = text_rect.x2;
            }
            rect.y2 += text_rect.y2;
        }
        else if (position == RTGUI_ICONBOX_TEXT_RIGHT)
        {
            rect.x2 += RTGUI_WIDGET_DEFAULT_MARGIN;
            if (text_rect.y2 > rect.y2)
            {
                rect.y2 = text_rect.y2;
            }
            rect.x2 += text_rect.x2;
        }

        /* set widget rect */
        rtgui_widget_set_rect(RTGUI_WIDGET(iconbox), &rect);

        /* set image and text position */
        iconbox->image = image;
        iconbox->text = (char *)rt_strdup((const char *)text);
        iconbox->text_position = position;
    }

    return iconbox;
}

void rtgui_iconbox_destroy(struct rtgui_iconbox *iconbox)
{
    rtgui_widget_destroy(RTGUI_WIDGET(iconbox));
}

void rtgui_iconbox_set_text_position(struct rtgui_iconbox *iconbox, int position)
{
    struct rtgui_rect rect = {0, 0, 0, 0}, text_rect;

    RT_ASSERT(iconbox != RT_NULL);

    iconbox->text_position = position;

    /* set mini width and height */
    rect.x2 = iconbox->image->w;
    rect.y2 = iconbox->image->h;

    /* get text rect */
    if (iconbox->text != RT_NULL)
    {
        rtgui_font_get_metrics(rtgui_font_default(),
                               iconbox->text, &text_rect);
        if (position == RTGUI_ICONBOX_TEXT_BELOW)
        {
            rect.y2 += RTGUI_WIDGET_DEFAULT_MARGIN;
            if (text_rect.x2 > rect.x2)
            {
                rect.x2 = text_rect.x2;
            }
            rect.y2 += text_rect.y2;
        }
        else if (position == RTGUI_ICONBOX_TEXT_RIGHT)
        {
            rect.x2 += RTGUI_WIDGET_DEFAULT_MARGIN;
            if (text_rect.y2 > rect.y2)
            {
                rect.y2 = text_rect.y2;
            }
            rect.x2 += text_rect.x2;
        }
    }
}

void rtgui_iconbox_set_selected(struct rtgui_iconbox *iconbox, rt_bool_t selected)
{
    RT_ASSERT(iconbox != RT_NULL);

    iconbox->selected = selected;
}
