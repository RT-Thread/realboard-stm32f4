#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>

/*rtgui_dc���ƻ���ͼ�κ���*/	
static void dc_draw(struct rtgui_widget *widget);
/*�������¼�����*/
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event);


/** 
  * @�������ƣ�event_handler()
  * @����    ��  �������¼�������
  * @����    ��  *object: �¼�����ָ��,*event:�¼�����ָ��
  * @����ֵ  �� rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{    
	/*��objectת����widget*/
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
	/*���������Ϣ*/
    rt_kprintf("event_handler\r\n");
	
    if(event->type == RTGUI_EVENT_PAINT)         //�¼�����ΪRTGUI_EVENT_PAINT
    {   
		/*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_PAINT\r\n");
		/*ִ�д����¼�*/
        rtgui_win_event_handler((struct rtgui_object*)object, event);
		/*���û��ƺ�������ͼ��*/
		dc_draw(widget);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //�¼�����ΪRTGUI_EVENT_SHOW
	{  
		/*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_SHOW\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
		
    }
    else if (event->type == RTGUI_EVENT_HIDE)  //�¼�����ΪRTGUI_EVENT_HIDE
    {    
		/*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_HIDE\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //�¼�����ΪRTGUI_EVENT_WIN_DEACTIVATE
    {   
		/*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //�¼�����ΪRTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
   else                                       //���ڷ�Χ�ڵ��¼�
    {
        rt_kprintf("event->type:%d\r\n", event->type);
        return rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    return RT_FALSE;
} 

/** 
  * @�������ƣ�  dc_draw()
  * @����    ��  dc��ͼ�����������ӵĺ���
  * @����    ��  *widget RTGUi����ָ��
  * @����ֵ  ��  None.
  */
static void dc_draw(struct rtgui_widget *widget)
{       
	    /*�ֲ�����*/
	     struct rtgui_dc *dc;
		 rtgui_color_t tcolor ;
	     rtgui_rect_t rect={100,100,400,200};
	    /*��ô����dc*/
        dc=rtgui_dc_begin_drawing(widget);
		/*��ŵ�ǰDC��ǰ��ɫ*/
		tcolor=RTGUI_DC_FC(dc);
		/*����ǰ��ɫ*/
	    RTGUI_DC_FC(dc)=RTGUI_RGB(0,0,200);
		/*����һ��ֱ��*/
		rtgui_dc_draw_line(dc,0,0,800,430);
		/*����Բ�Ǿ���*/
		rtgui_dc_draw_round_rect(dc,&rect,5);
		/*����ǰ��ɫ*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0,0);
		/*������Բ*/
		rtgui_dc_draw_ellipse(dc,400,200,200,100);
		/*����ǰ��ɫ*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0xff,0);
		/*�������Բ*/
		rtgui_dc_fill_circle(dc,600,200,100);
		/*����ǰ��ɫ*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0x00,0xff);
		/*���þ���*/
		rect.x1=10;
		rect.y1=300;
		rect.x2=rect.x1+100;
		rect.y2=rect.y1+50;
		/*����������*/
		rtgui_dc_fill_rect(dc,&rect);
		/*���þ���*/
		rect.x1=350;
		rect.y1=380;
		rect.x2=rect.x1+100;
		rect.y2=rect.y1+20;
		/*����ǰ��ɫ*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0x00,0x00);
		/*�����ı�*/
		rtgui_dc_draw_text(dc,"12345,hello",&rect);
		/*��ԭ����ǰ��ɫ���û�ȥ*/
		RTGUI_DC_FC(dc) =tcolor;
		/*����dc��ͼ*/
		rtgui_dc_end_drawing(dc);
		
}

void main(void)
{   
	/*�ֲ�����*/
    struct rtgui_app* application;
    rtgui_win_t *win;

    /*����Ӧ�ó���*/
	application = rtgui_app_create("dcdraw test");
	/*�ж��Ƿ�ɹ�����Ӧ�ó���*/
	if (RT_NULL==application)
	{
	 rt_kprintf("create application \"dcdraw test\" failed!\n");
	 rtgui_app_destroy(application);
	 return ;
	}
	/*����������*/
	win = rtgui_mainwin_create(RT_NULL, "dcdraw",
			RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
	/*�ж��Ƿ�ɹ�����������*/
	if(RT_NULL==win)
    {
         rt_kprintf("create main window failed!\n");
         rtgui_win_destroy(application);
		 rtgui_app_destroy(application);
	     return ;
    }
	/*��������¼�*/
	rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*��ʾ������*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
