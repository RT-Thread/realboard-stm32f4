#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

#include "ui_hello.h"

static void ui_thread_entry(void* parameter)
{
	struct rtgui_app* app;
	struct rtgui_win *win;
	struct rtgui_label *label;
    struct rtgui_box *box;

    /* create GUI application */
	app = rtgui_app_create("UiApp");
	RT_ASSERT(app != RT_NULL);

	/* create main window */
    win = rtgui_mainwin_create(RT_NULL, "UiWindow", RTGUI_WIN_STYLE_DEFAULT);

    /* we use layout engine to place sub-widgets */
    box = rtgui_box_create(RTGUI_HORIZONTAL, 10);
    rtgui_container_set_box(RTGUI_CONTAINER(win), box);

    /* create the 'hello world' label */
	label = rtgui_label_create("Hello World");
	rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

    rtgui_container_layout(RTGUI_CONTAINER(win));

	rtgui_win_show(win, RT_FALSE);
	rtgui_app_run(app);

	rtgui_win_destroy(win);
	rtgui_app_destroy(app);
}

int ui_hello(void)
{
	rt_thread_t tid;

    tid = rt_thread_create("UiApp", ui_thread_entry, RT_NULL, 
        2048, 20, 20);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

	return 0;
}
