/*
 * �����嵥���б���ͼ��ʾ
 *
 * ������ӻ��ȴ�����һ����ʾ�õ�view�����������İ�ťʱ�ᰴ��ģʽ��ʾ����ʽ��ʾ
 * �µ��б���ͼ
 */
#include <rtthread.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/filelist_view.h>

void main(void)
{
	struct rtgui_app* application;
	struct rtgui_win* win;	

	application = rtgui_app_create("filelist");
	if (application != RT_NULL)
	{			
		struct rtgui_rect rect;
	    rtgui_filelist_view_t *view;

		win = rtgui_mainwin_create(RT_NULL, "filelist", 
			RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
        
		rtgui_widget_get_extent(RTGUI_WIDGET(win), &rect);

	    view = rtgui_filelist_view_create("/", "*.*", &rect);
        rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(view));

		rtgui_win_show(win, RT_TRUE);
		rtgui_app_destroy(application);
	}
}
