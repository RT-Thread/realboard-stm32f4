/*
 * �����嵥��������ʾ
 *
 * ������ӻ��ȴ�����һ����ʾ�õ�container�����������İ�ťʱ�᲻ͬ��ģʽ��������
 */

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>
#include "demo_view.h"
#include <string.h>

static struct rtgui_timer *timer;
static struct rtgui_label *label;
static struct rtgui_win *autowin = RT_NULL;
static char label_text[80];
static rt_uint8_t cnt = 5;

rtgui_win_t *normal_window;
rtgui_label_t *normal_window_label;
static char normal_window_label_text[16];
static unsigned char normal_window_show_count = 1;
extern struct rtgui_win *main_win;

static rt_bool_t normal_window_onclose(struct rtgui_object *win,
                                       struct rtgui_event *event)
{
    normal_window_show_count++;

    return RT_TRUE;
}

static void create_normal_win(void)
{
    rtgui_rect_t rect = {30, 40, 150, 80};

    normal_window = rtgui_win_create(RT_NULL, "��ͨ����",
                                     &rect, RTGUI_WIN_STYLE_DEFAULT);

    rect.x1 += 20;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;

    /* ���һ���ı���ǩ */
    rt_sprintf(normal_window_label_text,
               "�� %d ����ʾ", normal_window_show_count);
    normal_window_label = rtgui_label_create(normal_window_label_text);
    rtgui_widget_set_rect(RTGUI_WIDGET(normal_window_label), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(normal_window),
                              RTGUI_WIDGET(normal_window_label));

    rtgui_win_set_onclose(normal_window,
                          normal_window_onclose);
}

/* ��������������ʾ */
static void demo_normal_window_onbutton(struct rtgui_object *object, rtgui_event_t *event)
{
    rt_sprintf(normal_window_label_text,
               "�� %d ����ʾ", normal_window_show_count);
    rtgui_label_set_text(normal_window_label,
                         normal_window_label_text);
    if (RTGUI_WIDGET_IS_HIDE(normal_window))
        rtgui_win_show(normal_window, RT_FALSE);
    else
        rtgui_win_activate(normal_window);
}

/* ��ȡһ�������Ĵ��ڱ��� */
static char *get_win_title()
{
    static rt_uint8_t win_no = 0;
    static char win_title[16];

    rt_sprintf(win_title, "���� %d", ++win_no);
    return win_title;
}

/* �رնԻ���ʱ�Ļص����� */
void diag_close(struct rtgui_timer *timer, void *parameter)
{
    cnt --;
    rt_sprintf(label_text, "closed then %d second!", cnt);

    /* ���ñ�ǩ�ı������¿ؼ� */
    rtgui_label_set_text(label, label_text);
    rtgui_widget_update(RTGUI_WIDGET(label));

    if (cnt == 0)
    {
        /* ��ʱ���رնԻ��� */
        rtgui_win_destroy(autowin);
    }
}

/* AUTO���ڹر�ʱ���¼����� */
rt_bool_t auto_window_close(struct rtgui_object *object, struct rtgui_event *event)
{
    if (timer != RT_NULL)
    {
        /* ֹͣ��ɾ����ʱ�� */
        rtgui_timer_stop(timer);
        rtgui_timer_destory(timer);

        timer = RT_NULL;
    }
    autowin = RT_NULL;

    return RT_TRUE;
}

static rt_uint16_t delta_x = 20;
static rt_uint16_t delta_y = 40;

/* �����Զ�������ʾ */
static void demo_autowin_onbutton(struct rtgui_object *object, rtgui_event_t *event)
{
    struct rtgui_rect rect = {50, 50, 200, 200};

    /* don't create the window twice */
    if (autowin)
        return;

    autowin = rtgui_win_create(main_win, "Information",
                              &rect, RTGUI_WIN_STYLE_DEFAULT | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    if (autowin == RT_NULL)
        return;

    cnt = 5;
    rt_sprintf(label_text, "closed then %d second!", cnt);
    label = rtgui_label_create(label_text);
    rect.x1 += 5;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(autowin),
                              RTGUI_WIDGET(label));

    /* ���ùرմ���ʱ�Ķ��� */
    rtgui_win_set_onclose(autowin, auto_window_close);

    rtgui_win_show(autowin, RT_FALSE);
    /* ����һ����ʱ�� */
    timer = rtgui_timer_create(100, RT_TIMER_FLAG_PERIODIC, diag_close, RT_NULL);
    rtgui_timer_start(timer);
}

/* ����ģ̬������ʾ */
static void demo_modalwin_onbutton(struct rtgui_object *object, rtgui_event_t *event)
{
    rtgui_win_t *win;
    rtgui_label_t *label;
    rtgui_rect_t rect = {0, 0, 150, 80};

    rtgui_rect_moveto(&rect, delta_x, delta_y);
    delta_x += 20;
    delta_y += 20;

    /* ����һ������ */
    win = rtgui_win_create(main_win,
                           get_win_title(), &rect, RTGUI_WIN_STYLE_DEFAULT);

    rect.x1 += 20;
    rect.x2 -= 5;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;

    label = rtgui_label_create("����һ��ģʽ����");
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

    /* ģ̬��ʾ���� */
    rtgui_win_show(win, RT_TRUE);

    /* ɾ�����Զ�ɾ������ */
    rtgui_win_destroy(win);
}

static void demo_close_ntitle_window(struct rtgui_object *object, rtgui_event_t *event)
{
    rtgui_win_t *win;

    /* ������ؼ� */
    win = RTGUI_WIN(rtgui_widget_get_toplevel(RTGUI_WIDGET(object)));

    /* ���ٴ��� */
    rtgui_win_destroy(win);
}

/* �����ޱ��ⴰ����ʾ */
static void demo_ntitlewin_onbutton(struct rtgui_object *object, rtgui_event_t *event)
{
    rtgui_win_t *win;
    rtgui_label_t *label;
    rtgui_button_t *button;
    rtgui_rect_t widget_rect, rect = {0, 0, 150, 80};

    rtgui_rect_moveto(&rect, delta_x, delta_y);
    delta_x += 20;
    delta_y += 20;

    /* ����һ�����ڣ����Ϊ�ޱ��⼰�ޱ߿� */
    win = rtgui_win_create(main_win,
                           "no title", &rect, RTGUI_WIN_STYLE_NO_TITLE |
                           RTGUI_WIN_STYLE_NO_BORDER |
                           RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    RTGUI_WIDGET_BACKGROUND(win) = white;

    /* ����һ���ı���ǩ */
    label = rtgui_label_create("�ޱ߿򴰿�");
    rtgui_font_get_metrics(RTGUI_WIDGET_FONT(label), "�ޱ߿򴰿�", &widget_rect);
    rtgui_rect_moveto_align(&rect, &widget_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
    widget_rect.y1 += 20;
    widget_rect.y2 += 20;
    rtgui_widget_set_rect(RTGUI_WIDGET(label), &widget_rect);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));
    RTGUI_WIDGET_BACKGROUND(label) = white;

    /* ����һ���رհ�ť */
    widget_rect.x1 = 0;
    widget_rect.y1 = 0;
    widget_rect.x2 = 40;
    widget_rect.y2 = 20;
    rtgui_rect_moveto_align(&rect, &widget_rect, RTGUI_ALIGN_CENTER_HORIZONTAL);
    widget_rect.y1 += 40;
    widget_rect.y2 += 40;
    button = rtgui_button_create("�ر�");
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &widget_rect);
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button));
    rtgui_button_set_onbutton(button, demo_close_ntitle_window);

    rtgui_win_show(win, RT_FALSE);
}

rtgui_container_t *demo_view_window(void)
{
    rtgui_rect_t  rect;
    rtgui_container_t *container;
    rtgui_button_t *button;

    /* ����һ����ʾ�õ���ͼ */
    container = demo_view("Window Demo");

    create_normal_win();

    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5;
    rect.y2 = rect.y1 + 20;
    /* ������ť������ʾ�������� */
    button = rtgui_button_create("Normal Win");
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(container), RTGUI_WIDGET(button));
    /* ����onbuttonΪdemo_win_onbutton���� */
    rtgui_button_set_onbutton(button, demo_normal_window_onbutton);

    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25;
    rect.y2 = rect.y1 + 20;
    /* ������ť������ʾһ���Զ��رյĴ��� */
    button = rtgui_button_create("Auto Win");
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(container), RTGUI_WIDGET(button));
    /* ����onbuttonΪdemo_autowin_onbutton���� */
    rtgui_button_set_onbutton(button, demo_autowin_onbutton);

    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ������ť���ڴ���һ��ģʽ���� */
    button = rtgui_button_create("Modal Win");
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(container), RTGUI_WIDGET(button));
    /* ����onbuttonΪdemo_modalwin_onbutton���� */
    rtgui_button_set_onbutton(button, demo_modalwin_onbutton);

    demo_view_get_rect(container, &rect);
    rect.x1 += 5;
    rect.x2 = rect.x1 + 100;
    rect.y1 += 5 + 25 + 25 + 25;
    rect.y2 = rect.y1 + 20;
    /* ������ť���ڴ���һ������������Ĵ��� */
    button = rtgui_button_create("NoTitle Win");
    rtgui_widget_set_rect(RTGUI_WIDGET(button), &rect);
    rtgui_container_add_child(RTGUI_CONTAINER(container), RTGUI_WIDGET(button));
    /* ����onbuttonΪdemo_ntitlewin_onbutton���� */
    rtgui_button_set_onbutton(button, demo_ntitlewin_onbutton);

    return container;
}
