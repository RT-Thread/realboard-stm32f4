#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>
#include <rtgui/widgets/button.h>

#include "ui_button.h"

static struct rtgui_label *label = RT_NULL;

static void onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	rt_kprintf("hello, button!\n");
	if (label != RT_NULL)
	{
		rtgui_label_set_text(label, "Hello from button");
		rtgui_widget_update(RTGUI_WIDGET(label));
	}

	return;
}

static void ui_thread_entry(void* parameter)
{
	struct rtgui_app* app;
	struct rtgui_win *win;
	struct rtgui_button *button;
    struct rtgui_box *box;

    /* create GUI application */
	app = rtgui_app_create("UiApp");
	RT_ASSERT(app != RT_NULL);

	/* create main window */
    win = rtgui_mainwin_create(RT_NULL, "UiWindow", RTGUI_WIN_STYLE_DEFAULT);

    /* we use layout engine to place sub-widgets */
    box = rtgui_box_create(RTGUI_VERTICAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);

    /* create the 'hello world' label */
	label = rtgui_label_create("Hello World");
	rtgui_widget_set_minwidth(RTGUI_WIDGET(label), 120);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

	/* create the button */
	button = rtgui_button_create("OK");
	rtgui_button_set_onbutton(button, onbutton);
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(button));

    rtgui_container_layout(RTGUI_CONTAINER(win));

	rtgui_win_show(win, RT_FALSE);
	rtgui_app_run(app);

	rtgui_win_destroy(win);
	rtgui_app_destroy(app);
}

int ui_button(void)
{
	rt_thread_t tid;

    tid = rt_thread_create("UiApp", ui_thread_entry, RT_NULL, 
        2048, 20, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

	return 0;
}
