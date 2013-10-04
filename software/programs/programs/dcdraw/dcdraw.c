#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>

/*rtgui_dc绘制基本图形函数*/	
static void dc_draw(struct rtgui_widget *widget);
/*主窗口事件函数*/
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event);


/** 
  * @函数名称：event_handler()
  * @描述    ：  主窗口事件服务函数
  * @参数    ：  *object: 事件对象指针,*event:事件类型指针
  * @返回值  ： rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{    
	/*将object转换成widget*/
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
	/*输出调试信息*/
    rt_kprintf("event_handler\r\n");
	
    if(event->type == RTGUI_EVENT_PAINT)         //事件类型为RTGUI_EVENT_PAINT
    {   
		/*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_PAINT\r\n");
		/*执行窗体事件*/
        rtgui_win_event_handler((struct rtgui_object*)object, event);
		/*调用绘制函数绘制图形*/
		dc_draw(widget);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //事件类型为RTGUI_EVENT_SHOW
	{  
		/*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_SHOW\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
		
    }
    else if (event->type == RTGUI_EVENT_HIDE)  //事件类型为RTGUI_EVENT_HIDE
    {    
		/*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_HIDE\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //事件类型为RTGUI_EVENT_WIN_DEACTIVATE
    {   
		/*输出调试信息*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\r\n");
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //事件类型为RTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
   else                                       //不在范围内的事件
    {
        rt_kprintf("event->type:%d\r\n", event->type);
        return rtgui_win_event_handler((struct rtgui_object*)object, event);
    }
    return RT_FALSE;
} 

/** 
  * @函数名称：  dc_draw()
  * @描述    ：  dc绘图函数，本例子的核心
  * @参数    ：  *widget RTGUi部件指针
  * @返回值  ：  None.
  */
static void dc_draw(struct rtgui_widget *widget)
{       
	    /*局部变量*/
	     struct rtgui_dc *dc;
		 rtgui_color_t tcolor ;
	     rtgui_rect_t rect={100,100,400,200};
	    /*获得窗体的dc*/
        dc=rtgui_dc_begin_drawing(widget);
		/*存放当前DC的前景色*/
		tcolor=RTGUI_DC_FC(dc);
		/*设置前景色*/
	    RTGUI_DC_FC(dc)=RTGUI_RGB(0,0,200);
		/*绘制一条直线*/
		rtgui_dc_draw_line(dc,0,0,800,430);
		/*绘制圆角矩形*/
		rtgui_dc_draw_round_rect(dc,&rect,5);
		/*设置前景色*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0,0);
		/*绘制椭圆*/
		rtgui_dc_draw_ellipse(dc,400,200,200,100);
		/*设置前景色*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0xff,0);
		/*绘制填充圆*/
		rtgui_dc_fill_circle(dc,600,200,100);
		/*设置前景色*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0x00,0xff);
		/*设置矩形*/
		rect.x1=10;
		rect.y1=300;
		rect.x2=rect.x1+100;
		rect.y2=rect.y1+50;
		/*绘制填充矩形*/
		rtgui_dc_fill_rect(dc,&rect);
		/*设置矩形*/
		rect.x1=350;
		rect.y1=380;
		rect.x2=rect.x1+100;
		rect.y2=rect.y1+20;
		/*设置前景色*/
		RTGUI_DC_FC(dc) = RTGUI_RGB(200,0x00,0x00);
		/*绘制文本*/
		rtgui_dc_draw_text(dc,"12345,hello",&rect);
		/*将原来的前景色设置回去*/
		RTGUI_DC_FC(dc) =tcolor;
		/*结束dc绘图*/
		rtgui_dc_end_drawing(dc);
		
}

void main(void)
{   
	/*局部变量*/
    struct rtgui_app* application;
    rtgui_win_t *win;

    /*建立应用程序*/
	application = rtgui_app_create("dcdraw test");
	/*判断是否成功建立应用程序*/
	if (RT_NULL==application)
	{
	 rt_kprintf("create application \"dcdraw test\" failed!\n");
	 rtgui_app_destroy(application);
	 return ;
	}
	/*建立主窗体*/
	win = rtgui_mainwin_create(RT_NULL, "dcdraw",
			RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
	/*判断是否成功建立主窗体*/
	if(RT_NULL==win)
    {
         rt_kprintf("create main window failed!\n");
         rtgui_win_destroy(application);
		 rtgui_app_destroy(application);
	     return ;
    }
	/*给窗体绑定事件*/
	rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*显示主窗体*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
