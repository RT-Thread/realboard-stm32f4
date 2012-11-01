#include <string.h>
#include <rtthread.h>

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/button.h>

#define LATTICE_SIZE        (20)

static rtgui_timer_t *timer;
static rt_size_t room_size_x, room_size_y;
static rt_size_t lattice_size_x, lattice_size_y;
static struct rtgui_rect room_rect, lattice_rect;

static void snake_draw(struct rtgui_widget *widget)
{
    struct rtgui_dc *dc;
    struct rtgui_rect rect;
    rt_uint32_t i;

    rt_kprintf("snake_draw\r\n");

    dc = rtgui_dc_begin_drawing(widget);
    if (dc == RT_NULL)
    {
        rt_kprintf("dc == RT_NULL\r\n");
        return;
    }

    rtgui_widget_get_rect(widget, &rect);
    rt_kprintf("rect => x1:%d x2:%d, y1:%d y2:%d\r\n", rect.x1, rect.x2, rect.y1, rect.y2);
//    rtgui_widget_rect_to_device(widget, &rect);
//    rt_kprintf("rect => x1:%d x2:%d, y1:%d y2:%d\r\n", rect.x1, rect.x2, rect.y1, rect.y2);

    // get room size.
    if((room_size_x == 0) || (room_size_y == 0))
    {
        rt_size_t tmp;

        room_size_x = rect.x2 - rect.x1;
        room_size_y = rect.y2 - rect.y1;
        memcpy(&room_rect, &rect, sizeof(struct rtgui_rect));
        rt_kprintf("room_rect => x1:%d x2:%d, y1:%d y2:%d\r\n",
                   room_rect.x1, room_rect.x2,
                   room_rect.y1, room_rect.y2);
        lattice_size_x = (room_rect.x2 - room_rect.x1) / LATTICE_SIZE;
        lattice_size_y = (room_rect.y2 - room_rect.y1) / LATTICE_SIZE;
        lattice_size_x -= 2;
        lattice_size_y -= 2;

        tmp = (room_rect.x2 - room_rect.x1) - (lattice_size_x * LATTICE_SIZE);
        lattice_rect.x1 = room_rect.x1 + (tmp / 2);
        lattice_rect.x2 = lattice_rect.x1 + (lattice_size_x * LATTICE_SIZE);

        tmp = (room_rect.y2 - room_rect.y1) - (lattice_size_y * LATTICE_SIZE);
        lattice_rect.y1 = room_rect.y1 + (tmp / 2);
        lattice_rect.y2 = lattice_rect.y1 + (lattice_size_y * LATTICE_SIZE);
        rt_kprintf("lattice_rect => x1:%d x2:%d, y1:%d y2:%d\r\n",
                   lattice_rect.x1, lattice_rect.x2,
                   lattice_rect.y1, lattice_rect.y2);
    }

    RTGUI_DC_BC(dc) = RTGUI_RGB(153, 153, 0);
    rtgui_dc_fill_rect(dc, &room_rect);// !!!
    RTGUI_DC_FC(dc) = RTGUI_RGB(255, 0, 0);
    rtgui_dc_draw_rect(dc, &lattice_rect);
    for(i=1; i<lattice_size_y; i++)
    {
        rtgui_dc_draw_horizontal_line(dc, lattice_rect.x1, lattice_rect.x2,
                                      lattice_rect.y1 + (LATTICE_SIZE * i));
    }
    for(i=1; i<lattice_size_x; i++)
    {
        rtgui_dc_draw_vertical_line(dc, lattice_rect.x1 + (LATTICE_SIZE * i),
                                    lattice_rect.y1, lattice_rect.y2);
    }

    rtgui_dc_end_drawing(dc);

    return;
}

static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);

    rt_kprintf("event_handler\r\n");

    if (event->type == RTGUI_EVENT_PAINT)
    {
        rt_kprintf("RTGUI_EVENT_PAINT\r\n");
        rtgui_container_event_handler(object, event);
        snake_draw(widget);
    }
    else if (event->type == RTGUI_EVENT_SHOW)
    {
        rt_kprintf("RTGUI_EVENT_SHOW\r\n");
        rtgui_container_event_handler(object, event);
        snake_draw(widget);
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_HIDE)
    {
        rt_kprintf("RTGUI_EVENT_HIDE\r\n");
        rtgui_container_event_handler(object, event);
        rtgui_timer_stop(timer);
    }
    else
    {
        rt_kprintf("DEF\r\n");
        return rtgui_container_event_handler(object, event);
    }

    return RT_FALSE;
}

static void timeout(struct rtgui_timer *timer, void *parameter)
{
    struct rtgui_dc *dc;
    struct rtgui_rect rect;
    struct rtgui_widget *widget;

//    rt_kprintf("sanke timeout\r\n");

    widget = RTGUI_WIDGET(parameter);

    dc = rtgui_dc_begin_drawing(widget);
    if (dc == RT_NULL)
    {
        rt_kprintf("dc == RT_NULL\r\n");
        return;
    }

//    rtgui_widget_get_rect(widget, &rect);
//    rt_kprintf("rect => x1:%d x2:%d, y1:%d y2:%d\r\n", rect.x1, rect.x2, rect.y1, rect.y2);
//    rtgui_widget_rect_to_device(widget, &rect);
//    rt_kprintf("rect => x1:%d x2:%d, y1:%d y2:%d\r\n", rect.x1, rect.x2, rect.y1, rect.y2);

//    rtgui_dc_fill_rect(dc, &text_rect);
//    rtgui_dc_fill_rect(dc, &rect);

//    RTGUI_DC_FC(dc) = 0xFF;
//    rtgui_dc_draw_circle(dc, rect.x1 + 10, rect.y1 + 10, 10);
//    rtgui_dc_draw_circle(dc, 0, 0, 10);

//    RTGUI_DC_FC(dc) = 0xFF;

//    RTGUI_DC_FC(dc) = RTGUI_RGB(25, 70, 150);
//    rtgui_dc_fill_rect(dc, &rect);
//
//    rect.y1 = 100;
//    rect.y2 = 120;
//    RTGUI_DC_FC(dc) = RTGUI_RGB(255, 70, 150);
//    rtgui_dc_draw_round_rect(dc, &rect, 10);
//
//    rtgui_dc_fill_circle(dc, rect.x1 + 30, rect.y1 + 10, 10);

    rtgui_dc_end_drawing(dc);

    return;
}

void main(void)
{
    struct rtgui_app* application;
    struct rtgui_win* win;

    application = rtgui_app_create(rt_thread_self(), "sanke_app");
    if (application != RT_NULL)
    {
        win = rtgui_mainwin_create(RT_NULL,
                                   "sanke_win",
                                   RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
        if(win == RT_NULL)
        {
            rt_kprintf("sanke_win == RT_NULL\r\n");
            return;
        }

        //rtgui_object_set_event_handler(RTGUI_OBJECT(container), event_handler);
        rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
//        timer = rtgui_timer_create(2, RT_TIMER_FLAG_PERIODIC, timeout, (void *)container);
        timer = rtgui_timer_create(RT_TICK_PER_SECOND*2, RT_TIMER_FLAG_PERIODIC, timeout, (void *)win);

        rtgui_win_show(win, RT_TRUE);
        //退出后才返回

        rtgui_app_destroy(application);
    }
}
