#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/label.h>

void main(void)
{
	struct rtgui_app* application;
	struct rtgui_win* win;	
	struct rtgui_label* label;

	application = rtgui_app_create(rt_thread_self(), "label");
	if (application != RT_NULL)
	{	
		rtgui_rect_t rect = {220, 250, 400, 450};
		win = rtgui_mainwin_create(RT_NULL, "Label", 
			RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);

		/* create lable in app window */
		label = rtgui_label_create("This is a RTGUI label Demo");
		rtgui_widget_set_rect(RTGUI_WIDGET(label), &rect);
		rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(label));

		rtgui_win_show(win, RT_TRUE);
		rtgui_app_destroy(application);
	}
}
