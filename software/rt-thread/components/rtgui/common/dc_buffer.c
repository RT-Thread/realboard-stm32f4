/*
 * File      : dc_buffer.c
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
#include <rtgui/rtgui.h>
#include <rtgui/dc.h>
#include <rtgui/blit.h>
#include <rtgui/dc_hw.h>
#include <rtgui/color.h>
#include <rtgui/rtgui_system.h>

#define hw_driver               (rtgui_graphic_driver_get_default())

static rt_bool_t rtgui_dc_buffer_fini(struct rtgui_dc *dc);
static void rtgui_dc_buffer_draw_point(struct rtgui_dc *dc, int x, int y);
static void rtgui_dc_buffer_draw_color_point(struct rtgui_dc *dc, int x, int y, rtgui_color_t color);
static void rtgui_dc_buffer_draw_vline(struct rtgui_dc *dc, int x, int y1, int y2);
static void rtgui_dc_buffer_draw_hline(struct rtgui_dc *dc, int x1, int x2, int y);
static void rtgui_dc_buffer_fill_rect(struct rtgui_dc *dc, struct rtgui_rect *rect);
static void rtgui_dc_buffer_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data);
static void rtgui_dc_buffer_blit(struct rtgui_dc *self, struct rtgui_point *dc_point,
                                 struct rtgui_dc *dest, rtgui_rect_t *rect);

static void rtgui_dc_buffer_set_gc(struct rtgui_dc *dc, rtgui_gc_t *gc);
static rtgui_gc_t *rtgui_dc_buffer_get_gc(struct rtgui_dc *dc);

static rt_bool_t rtgui_dc_buffer_get_visible(struct rtgui_dc *dc);
static void rtgui_dc_buffer_get_rect(struct rtgui_dc *dc, rtgui_rect_t *rect);

const static struct rtgui_dc_engine dc_buffer_engine =
{
    rtgui_dc_buffer_draw_point,
    rtgui_dc_buffer_draw_color_point,
    rtgui_dc_buffer_draw_vline,
    rtgui_dc_buffer_draw_hline,
    rtgui_dc_buffer_fill_rect,
    rtgui_dc_buffer_blit_line,
    rtgui_dc_buffer_blit,

    rtgui_dc_buffer_set_gc,
    rtgui_dc_buffer_get_gc,

    rtgui_dc_buffer_get_visible,
    rtgui_dc_buffer_get_rect,

    rtgui_dc_buffer_fini,
};

struct rtgui_dc *rtgui_dc_buffer_create(int w, int h)
{
    struct rtgui_dc_buffer *dc;

    dc = (struct rtgui_dc_buffer *)rtgui_malloc(sizeof(struct rtgui_dc_buffer));
    dc->parent.type   = RTGUI_DC_BUFFER;
    dc->parent.engine = &dc_buffer_engine;
    dc->gc.foreground = default_foreground;
    dc->gc.background = default_background;
    dc->gc.font = rtgui_font_default();
    dc->gc.textalign = RTGUI_ALIGN_LEFT | RTGUI_ALIGN_TOP;

    dc->width   = w;
    dc->height  = h;
    dc->pitch   = w *sizeof(rtgui_color_t);

    rtgui_region_init(&(dc->clip));

    dc->pixel = rtgui_malloc(h * dc->pitch);
    rt_memset(dc->pixel, 0, h * dc->pitch);

    return &(dc->parent);
}

rt_uint8_t *rtgui_dc_buffer_get_pixel(struct rtgui_dc *dc)
{
    struct rtgui_dc_buffer *dc_buffer;

    dc_buffer = (struct rtgui_dc_buffer *)dc;

    return dc_buffer->pixel;
}

static rt_bool_t rtgui_dc_buffer_fini(struct rtgui_dc *dc)
{
    struct rtgui_dc_buffer *buffer = (struct rtgui_dc_buffer *)dc;

    if (dc->type != RTGUI_DC_BUFFER) return RT_FALSE;

    rtgui_free(buffer->pixel);
    buffer->pixel = RT_NULL;

    return RT_TRUE;
}

static void rtgui_dc_buffer_draw_point(struct rtgui_dc *self, int x, int y)
{
    rtgui_color_t *ptr;
    struct rtgui_dc_buffer *dc;

    dc = (struct rtgui_dc_buffer *)self;

    /* does not draw point out of dc */
    if ((x > dc->width) || (y > dc->height)) return ;

    /* note: there is no parameter check in this function */
    ptr = (rtgui_color_t *)(dc->pixel + y * dc->pitch + x * sizeof(rtgui_color_t));

    *ptr = dc->gc.foreground;
}

static void rtgui_dc_buffer_draw_color_point(struct rtgui_dc *self, int x, int y, rtgui_color_t color)
{
    rtgui_color_t *ptr;
    struct rtgui_dc_buffer *dc;

    dc = (struct rtgui_dc_buffer *)self;

    /* note: there is no parameter check in this function */
    ptr = (rtgui_color_t *)(dc->pixel + y * dc->pitch + x * sizeof(rtgui_color_t));

    *ptr = color;
}

static void rtgui_dc_buffer_draw_vline(struct rtgui_dc *self, int x, int y1, int y2)
{
    rtgui_color_t *ptr;
    register rt_base_t index;
    struct rtgui_dc_buffer *dc;

    dc = (struct rtgui_dc_buffer *)self;

    if (x  >= dc->width) return;
    if (y1 > dc->height) y1 = dc->height;
    if (y2 > dc->height) y2 = dc->height;

    ptr = (rtgui_color_t *)(dc->pixel + y1 * dc->pitch + x * sizeof(rtgui_color_t));
    for (index = y1; index < y2; index ++)
    {
        /* draw this point */
        *ptr = dc->gc.foreground;
        ptr += dc->width;
    }
}

static void rtgui_dc_buffer_draw_hline(struct rtgui_dc *self, int x1, int x2, int y)
{
    rtgui_color_t *ptr;
    register rt_base_t index;
    struct rtgui_dc_buffer *dc;

    dc = (struct rtgui_dc_buffer *)self;
    if (y >= dc->height) return;
    if (x1 > dc->width) x1 = dc->width;
    if (x2 > dc->width) x2 = dc->width;

    ptr = (rtgui_color_t *)(dc->pixel + y * dc->pitch + x1 * sizeof(rtgui_color_t));
    for (index = x1; index < x2; index ++)
    {
        /* draw this point */
        *ptr++ = dc->gc.foreground;
    }
}

static void rtgui_dc_buffer_fill_rect(struct rtgui_dc *self, struct rtgui_rect *rect)
{
    rtgui_color_t foreground;
    rtgui_rect_t r;
    struct rtgui_dc_buffer *dc;

    r = *rect;
    dc = (struct rtgui_dc_buffer *)self;
    if (r.x1 > dc->width) r.x1 = dc->width;
    if (r.x2 > dc->width) r.x2 = dc->width;
    if (r.y1 > dc->height) r.y1 = dc->height;
    if (r.y2 > dc->height) r.y2 = dc->height;

    /* save foreground color */
    foreground = RTGUI_DC_FC(self);

    /* set background color as foreground color */
    RTGUI_DC_FC(self) = RTGUI_DC_BC(self);

    /* fill first line */
    rtgui_dc_buffer_draw_hline(&(dc->parent), r.x1, r.x2, r.y1);

    /* memory copy other lines */
    if (r.y2 > r.y1)
    {
        register rt_base_t index;
        for (index = r.y1 + 1; index < r.y2; index ++)
        {
            rt_memcpy(dc->pixel + index * dc->pitch,
                      dc->pixel + r.y1 * dc->pitch,
                      (r.x2 - r.x1) * sizeof(rtgui_color_t));
        }
    }

    /* restore foreground color */
    RTGUI_DC_FC(self) = foreground;
}

/* blit a dc to a hardware dc */
static void rtgui_dc_buffer_blit(struct rtgui_dc *self, struct rtgui_point *dc_point, struct rtgui_dc *dest, rtgui_rect_t *rect)
{
    struct rtgui_dc_buffer *dc = (struct rtgui_dc_buffer *)self;

    if (dc_point == RT_NULL) dc_point = &rtgui_empty_point;
    if (rtgui_dc_get_visible(dest) == RT_FALSE) return;

    if ((dest->type == RTGUI_DC_HW) || (dest->type == RTGUI_DC_CLIENT))
    {
        rt_uint8_t *line_ptr, *pixels;
        rt_uint16_t rect_width, rect_height, index, pitch;
        rtgui_blit_line_func blit_line;

        /* calculate correct width and height */
        if (rtgui_rect_width(*rect) > (dc->width - dc_point->x))
            rect_width = dc->width - dc_point->x;
        else
            rect_width = rtgui_rect_width(*rect);

        if (rtgui_rect_height(*rect) > (dc->height - dc_point->y))
            rect_height = dc->height - dc_point->y;
        else
            rect_height = rtgui_rect_height(*rect);

        /* prepare pixel line */
        pixels = dc->pixel + dc_point->y * dc->pitch + dc_point->x * sizeof(rtgui_color_t);

        if (hw_driver->bits_per_pixel == sizeof(rtgui_color_t) * 8)
        {
            /* it's the same byte per pixel, draw it directly */
            for (index = rect->y1; index < rect->y1 + rect_height; index++)
            {
                dest->engine->blit_line(dest, rect->x1, rect->x1 + rect_width, index, pixels);
                pixels += dc->width * sizeof(rtgui_color_t);
            }
        }
        else
        {
            /* get blit line function */
            blit_line = rtgui_blit_line_get(hw_driver->bits_per_pixel / 8, 4);
            /* calculate pitch */
            pitch = rect_width * sizeof(rtgui_color_t);
            /* create line buffer */
            line_ptr = (rt_uint8_t *) rtgui_malloc(rect_width * hw_driver->bits_per_pixel / 8);

            /* draw each line */
            for (index = rect->y1; index < rect->y1 + rect_height; index ++)
            {
                /* blit on line buffer */
                blit_line(line_ptr, (rt_uint8_t *)pixels, pitch);
                pixels += dc->width * sizeof(rtgui_color_t);

                /* draw on hardware dc */
                dest->engine->blit_line(dest, rect->x1, rect->x1 + rect_width, index, line_ptr);
            }

            /* release line buffer */
            rtgui_free(line_ptr);
        }
    }
}

static void rtgui_dc_buffer_blit_line(struct rtgui_dc *self, int x1, int x2, int y, rt_uint8_t *line_data)
{
    rtgui_color_t *color_ptr;
    struct rtgui_dc_buffer *dc = (struct rtgui_dc_buffer *)self;

    RT_ASSERT(dc != RT_NULL);
    RT_ASSERT(line_data != RT_NULL);

    /* out of range */
    if ((x1 > dc->width) || (y > dc->height)) return;
    /* check range */
    if (x2 > dc->width) x2 = dc->width;

    color_ptr = (rtgui_color_t *)(dc->pixel + y * dc->pitch + x1 * sizeof(rtgui_color_t));
    rt_memcpy(color_ptr, line_data, (x2 - x1) * sizeof(rtgui_color_t));
}

static void rtgui_dc_buffer_set_gc(struct rtgui_dc *self, rtgui_gc_t *gc)
{
    struct rtgui_dc_buffer *dc = (struct rtgui_dc_buffer *)self;

    dc->gc = *gc;
}

static rtgui_gc_t *rtgui_dc_buffer_get_gc(struct rtgui_dc *self)
{
    struct rtgui_dc_buffer *dc = (struct rtgui_dc_buffer *)self;

    return &dc->gc;
}

static rt_bool_t rtgui_dc_buffer_get_visible(struct rtgui_dc *dc)
{
    return RT_TRUE;
}

static void rtgui_dc_buffer_get_rect(struct rtgui_dc *self, rtgui_rect_t *rect)
{
    struct rtgui_dc_buffer *dc = (struct rtgui_dc_buffer *)self;

    rect->x1 = rect->y1 = 0;

    rect->x2 = dc->width;
    rect->y2 = dc->height;
}

