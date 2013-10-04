#include <rtgui/rtgui.h>
#include <rtgui/dc.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/calibration.h>

static rt_bool_t (*_cali_restore)(void);
void calibration_set_restore(rt_bool_t (*calibration_restore)(void))
{
    _cali_restore = calibration_restore;
}

static void (*_cali_after)(struct calibration_data *data);
void calibration_set_after(void (*calibration_after)(struct calibration_data *data))
{
    _cali_after = calibration_after;
}

#define CALIBRATION_STEP_LEFTTOP        0
#define CALIBRATION_STEP_RIGHTTOP       1
#define CALIBRATION_STEP_RIGHTBOTTOM    2
#define CALIBRATION_STEP_LEFTBOTTOM     3
#define CALIBRATION_STEP_CENTER         4

#define TOUCH_WIN_UPDATE    1
#define TOUCH_WIN_CLOSE     2

#define CALIBRATION_WIDTH   15
#define CALIBRATION_HEIGHT  15

struct calibration_session
{
    rt_uint8_t step;

    struct calibration_data data;

    rt_uint16_t width;
    rt_uint16_t height;

    rtgui_win_t *win;

    rt_device_t device;

    struct rtgui_app *app;
};
static struct calibration_session *calibration_ptr = RT_NULL;

static void calibration_data_post(rt_uint16_t x, rt_uint16_t y)
{
    if (calibration_ptr == RT_NULL)
        return;

    switch (calibration_ptr->step)
    {
    case CALIBRATION_STEP_LEFTTOP:
        calibration_ptr->data.min_x = x;
        calibration_ptr->data.min_y = y;
        break;

    case CALIBRATION_STEP_RIGHTTOP:
        calibration_ptr->data.max_x = x;
        calibration_ptr->data.min_y = (calibration_ptr->data.min_y + y) / 2;
        break;

    case CALIBRATION_STEP_LEFTBOTTOM:
        calibration_ptr->data.min_x = (calibration_ptr->data.min_x + x) / 2;
        calibration_ptr->data.max_y = y;
        break;

    case CALIBRATION_STEP_RIGHTBOTTOM:
        calibration_ptr->data.max_x = (calibration_ptr->data.max_x + x) / 2;
        calibration_ptr->data.max_y = (calibration_ptr->data.max_y + y) / 2;
        break;

    case CALIBRATION_STEP_CENTER:
        /* calibration done */
        {
            rt_uint16_t w, h;

            struct rtgui_event_command ecmd;
            RTGUI_EVENT_COMMAND_INIT(&ecmd);
            ecmd.wid = calibration_ptr->win;
            ecmd.command_id = TOUCH_WIN_CLOSE;

            /* calculate calibrated data */
            if (calibration_ptr->data.max_x > calibration_ptr->data.min_x)
                w = calibration_ptr->data.max_x - calibration_ptr->data.min_x;
            else
                w = calibration_ptr->data.min_x - calibration_ptr->data.max_x;
            w = (w / (calibration_ptr->width - 2 * CALIBRATION_WIDTH)) * CALIBRATION_WIDTH;

            if (calibration_ptr->data.max_y > calibration_ptr->data.min_y)
                h = calibration_ptr->data.max_y - calibration_ptr->data.min_y;
            else
                h = calibration_ptr->data.min_y - calibration_ptr->data.max_y;
            h = (h / (calibration_ptr->height - 2 * CALIBRATION_HEIGHT)) * CALIBRATION_HEIGHT;

            if (calibration_ptr->data.max_x > calibration_ptr->data.min_x)
            {
                calibration_ptr->data.min_x -= w;
                calibration_ptr->data.max_x += w;
            }
            else
            {
                calibration_ptr->data.min_x += w;
                calibration_ptr->data.max_x -= w;
            }

            if (calibration_ptr->data.max_y > calibration_ptr->data.min_y)
            {
                calibration_ptr->data.min_y -= h;
                calibration_ptr->data.max_y += h;
            }
            else
            {
                calibration_ptr->data.min_y += h;
                calibration_ptr->data.max_y -= h;
            }

            rtgui_send(calibration_ptr->app, &ecmd.parent, sizeof(struct rtgui_event_command));
        }
        calibration_ptr->step = 0;
        return;
    }

    calibration_ptr->step++;

    /* post command event */
    {
        struct rtgui_event_command ecmd;
        RTGUI_EVENT_COMMAND_INIT(&ecmd);
        ecmd.wid = calibration_ptr->win;
        ecmd.command_id = TOUCH_WIN_UPDATE;

        rtgui_send(calibration_ptr->app, &ecmd.parent, sizeof(struct rtgui_event_command));
    }
}

static rt_bool_t calibration_event_handler(struct rtgui_object *object, struct rtgui_event *event)
{
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
    rtgui_rect_t label_rect = {120, 250, 680, 270};
    switch (event->type)
    {
    case RTGUI_EVENT_PAINT:
    {
        struct rtgui_dc *dc;
        struct rtgui_rect rect;

        dc = rtgui_dc_begin_drawing(widget);
        if (dc == RT_NULL) break;

        /* get rect information */
        rtgui_widget_get_rect(widget, &rect);

        /* clear whole window */
        RTGUI_WIDGET_BACKGROUND(widget) = white;
        rtgui_dc_fill_rect(dc, &rect);

        /* reset color */
        RTGUI_WIDGET_BACKGROUND(widget) = green;
        RTGUI_WIDGET_FOREGROUND(widget) = black;

        switch (calibration_ptr->step)
        {
        case CALIBRATION_STEP_LEFTTOP:
            rtgui_dc_draw_hline(dc,
                                0,
                                2 * CALIBRATION_WIDTH,
                                CALIBRATION_HEIGHT);
            rtgui_dc_draw_vline(dc,
                                CALIBRATION_WIDTH,
                                0,
                                2 * CALIBRATION_HEIGHT);
            rtgui_dc_draw_text(dc,
                               "Please touch the sight bead on the lift top to finish Calibration!",
                               &label_rect);
            RTGUI_WIDGET_FOREGROUND(widget) = red;
            rtgui_dc_fill_circle(dc,
                                 CALIBRATION_WIDTH,
                                 CALIBRATION_HEIGHT,
                                 4);
            break;

        case CALIBRATION_STEP_RIGHTTOP:
            rtgui_dc_draw_hline(dc,
                                calibration_ptr->width - 2 * CALIBRATION_WIDTH,
                                calibration_ptr->width,
                                CALIBRATION_HEIGHT);
            rtgui_dc_draw_vline(dc,
                                calibration_ptr->width - CALIBRATION_WIDTH,
                                0,
                                2 * CALIBRATION_HEIGHT);
            rtgui_dc_draw_text(dc,
                               "Please touch the sight bead on the right top to finish Calibration!",
                               &label_rect);
            RTGUI_WIDGET_FOREGROUND(widget) = red;
            rtgui_dc_fill_circle(dc,
                                 calibration_ptr->width - CALIBRATION_WIDTH,
                                 CALIBRATION_HEIGHT,
                                 4);
            break;

        case CALIBRATION_STEP_LEFTBOTTOM:
            rtgui_dc_draw_hline(dc,
                                0,
                                2 * CALIBRATION_WIDTH,
                                calibration_ptr->height - CALIBRATION_HEIGHT);
            rtgui_dc_draw_vline(dc,
                                CALIBRATION_WIDTH,
                                calibration_ptr->height - 2 * CALIBRATION_HEIGHT,
                                calibration_ptr->height);
            rtgui_dc_draw_text(dc,
                               "Please touch the sight bead on the left bottom to finish Calibration!",
                               &label_rect);
            RTGUI_WIDGET_FOREGROUND(widget) = red;
            rtgui_dc_fill_circle(dc,
                                 CALIBRATION_WIDTH,
                                 calibration_ptr->height - CALIBRATION_HEIGHT,
                                 4);
            break;

        case CALIBRATION_STEP_RIGHTBOTTOM:
            rtgui_dc_draw_hline(dc,
                                calibration_ptr->width - 2 * CALIBRATION_WIDTH,
                                calibration_ptr->width,
                                calibration_ptr->height - CALIBRATION_HEIGHT);
            rtgui_dc_draw_vline(dc,
                                calibration_ptr->width - CALIBRATION_WIDTH,
                                calibration_ptr->height - 2 * CALIBRATION_HEIGHT,
                                calibration_ptr->height);
            rtgui_dc_draw_text(dc,
                               "Please touch the sight bead on the right bottom to finish Calibration!",
                               &label_rect);
            RTGUI_WIDGET_FOREGROUND(widget) = red;
            rtgui_dc_fill_circle(dc,
                                 calibration_ptr->width - CALIBRATION_WIDTH,
                                 calibration_ptr->height - CALIBRATION_HEIGHT,
                                 4);
            break;

        case CALIBRATION_STEP_CENTER:
            rtgui_dc_draw_hline(dc,
                                calibration_ptr->width / 2 - CALIBRATION_WIDTH,
                                calibration_ptr->width / 2 + CALIBRATION_WIDTH,
                                calibration_ptr->height / 2);
            rtgui_dc_draw_vline(dc,
                                calibration_ptr->width / 2,
                                calibration_ptr->height / 2 - CALIBRATION_HEIGHT,
                                calibration_ptr->height / 2 + CALIBRATION_HEIGHT);
            rtgui_dc_draw_text(dc,
                               "Please touch the sight bead on the center to finish Calibration!",
                               &label_rect);
            RTGUI_WIDGET_FOREGROUND(widget) = red;
            rtgui_dc_fill_circle(dc,
                                 calibration_ptr->width / 2,
                                 calibration_ptr->height / 2,
                                 4);
            break;
        }
        rtgui_dc_end_drawing(dc);
    }
    break;

    case RTGUI_EVENT_COMMAND:
    {
        struct rtgui_event_command *ecmd = (struct rtgui_event_command *)event;

        switch (ecmd->command_id)
        {
        case TOUCH_WIN_UPDATE:
            rtgui_widget_update(widget);
            break;
        case TOUCH_WIN_CLOSE:
            rtgui_win_close(RTGUI_WIN(widget));
            break;
        }
    }
    return RT_TRUE;

    default:
        rtgui_win_event_handler(RTGUI_OBJECT(widget), event);
    }

    return RT_FALSE;
}

static void calibration_entry(void *parameter)
{
    rt_device_t device;
    struct rtgui_rect rect;

    device = rt_device_find("touch");
    if (device == RT_NULL)
    {
        rt_kprintf("RTGUI: no touch device to calibrate\n");
        return;
    }

    calibration_ptr = (struct calibration_session *)
                      rt_malloc(sizeof(*calibration_ptr));
    rt_memset(calibration_ptr, 0, sizeof(*calibration_ptr));
    calibration_ptr->device = device;

    rt_device_control(calibration_ptr->device, RT_TOUCH_CALIBRATION,
                      (void *)calibration_data_post);

    rtgui_graphic_driver_get_rect(rtgui_graphic_driver_get_default(), &rect);

    /* set screen rect */
    calibration_ptr->width = rect.x2;
    calibration_ptr->height = rect.y2;

    calibration_ptr->app = rtgui_app_create("calibration");
    if (calibration_ptr->app == RT_NULL)
    {
        rt_kprintf("RTGUI: no mem to create calibration app\n");
        goto __free_ptr;
    }

    /* create calibration window */
    calibration_ptr->win = rtgui_win_create(RT_NULL,
                                            "calibration", &rect,
                                            RTGUI_WIN_STYLE_NO_TITLE | RTGUI_WIN_STYLE_NO_BORDER |
                                            RTGUI_WIN_STYLE_ONTOP | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    if (calibration_ptr->win != RT_NULL)
    {
        rtgui_object_set_event_handler(RTGUI_OBJECT(calibration_ptr->win),
                calibration_event_handler);
        rtgui_win_show(calibration_ptr->win, RT_TRUE);
    }

    rtgui_app_destroy(calibration_ptr->app);

    /* set calibration data */
    rt_device_control(calibration_ptr->device,
                      RT_TOUCH_CALIBRATION_DATA,
                      &calibration_ptr->data);

    if (_cali_after)
        _cali_after(&calibration_ptr->data);

    /* recover to normal */
    rt_device_control(calibration_ptr->device, RT_TOUCH_NORMAL, RT_NULL);

__free_ptr:
    /* release memory */
    rt_free(calibration_ptr);
    calibration_ptr = RT_NULL;
}

void calibration_init(void)
{
    rt_thread_t tid;

    rt_device_t device = rt_device_find("touch");

    if (device == RT_NULL)
    {
        rt_kprintf("RTGUI: no touch device to calibrate\n");
        return;
    }

    if (_cali_restore && _cali_restore())
    {
        return;
    }

    tid = rt_thread_create("cali", calibration_entry, RT_NULL, 1024, 20, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
void calibration(void)
{
    calibration_init();
}
FINSH_FUNCTION_EXPORT(calibration, perform touch calibration);
#endif
