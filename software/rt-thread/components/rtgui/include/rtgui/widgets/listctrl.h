/*
 * File      : listctrl.h
 * This file is part of RTGUI in RT-Thread RTOS
 * COPYRIGHT (C) 2010, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-12-06     Bernard      first version
 */

#ifndef __RTGUI_LISTCTRL_H__
#define __RTGUI_LISTCTRL_H__

#include <rtgui/rtgui.h>
#include <rtgui/image.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/widget.h>

DECLARE_CLASS_TYPE(listctrl);
/** Gets the type of a listctrl */
#define RTGUI_LISTCTRL_TYPE     (RTGUI_TYPE(listctrl))
/** Casts the object to a listctrl */
#define RTGUI_LISTCTRL(obj)     (RTGUI_OBJECT_CAST((obj), RTGUI_LISTCTRL_TYPE, rtgui_listctrl_t))
/** Checks if the object is a listctrl */
#define RTGUI_IS_LISTCTRL(obj)  (RTGUI_OBJECT_CHECK_TYPE((obj), RTGUI_LISTCTRL_TYPE))

struct rtgui_listctrl
{
    struct rtgui_widget parent;

    /* widget private data */
    /* listctrl items */
    void *items;

    /* total number of items */
    rt_uint16_t items_count;
    /* the number of item in a page */
    rt_uint16_t page_items;
    /* current item */
    rt_int16_t current_item;
    rt_uint16_t item_height;

    /* item event handler */
    rtgui_event_handler_ptr on_item;
    void (*on_item_draw)(struct rtgui_listctrl *list, struct rtgui_dc *dc, rtgui_rect_t *rect, rt_uint16_t index);
};
typedef struct rtgui_listctrl rtgui_listctrl_t;

typedef void (*rtgui_onitem_draw_t)(struct rtgui_listctrl *list, struct rtgui_dc *dc, rtgui_rect_t *rect, rt_uint16_t index);

rtgui_listctrl_t *rtgui_listctrl_create(void *items, rt_uint16_t count,
                                        rtgui_rect_t *rect, rtgui_onitem_draw_t ondraw);
void rtgui_listctrl_destroy(rtgui_listctrl_t *ctrl);

rt_bool_t rtgui_listctrl_event_handler(struct rtgui_object *object, struct rtgui_event *event);
void rtgui_listctrl_set_onitem(rtgui_listctrl_t *ctrl, rtgui_event_handler_ptr func);
void rtgui_listctrl_set_items(rtgui_listctrl_t *ctrl, void *items, rt_uint16_t count);
void rtgui_listctrl_set_current_item(struct rtgui_listctrl *ctrl, rt_uint16_t index);
rt_bool_t rtgui_listctrl_get_item_rect(rtgui_listctrl_t *ctrl, rt_uint16_t item, rtgui_rect_t *item_rect);
void rtgui_listctrl_set_itemheight(struct rtgui_listctrl *ctrl, int height);

#endif
