/*
	2014-2-22  jiezhi320  实现简易计算器    
        app形式运行浮点数有问题，所以改为int型计算
*/


#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/widgets/button.h>
#include <rtgui/image.h>
#include <rtgui/widgets/label.h>

#include <rtgui/rtgui_app.h>
#include "string.h"
#include "stdlib.h"
#include "math.h"

struct rtgui_label *label_result;

typedef enum _OPERATOR
{
    PLUS,	//+
    MINUS,	//-
    MULTI,  //*
    DIV,    // /
    PER     //%
} OPERATOR;

static OPERATOR opert;  //运算符

#define LEN	95
static char char_display[LEN]; //输入捕获用
static char char_res[LEN];     //显示用

static int  step;         //输入步骤
static int  point_flag;

#define C_NUM		1
#define C_OPERT		2
#define C_POINT		3
int input_type(char c)
{
	if (c=='.') return C_POINT;
	if (c=='+') return C_OPERT;
	if (c=='-') return C_OPERT;	
	if (c=='*') return C_OPERT;		
	if (c=='/') return C_OPERT;		
	if (c=='%') return C_OPERT;		
	return C_NUM;
}

static void char_res_clr(void)
{
	rt_memset(char_display, ' ', sizeof(char_display));
	char_display[LEN-1]=0;
	
	rtgui_label_set_text(label_result, char_display);
	step = 0;
	point_flag = 0;
}

void cal(void)
{
	static volatile int/*double*/ f_var1; //第一个操作数
	static volatile int/*double*/ f_var2; //第二个操作数
	static volatile int/*double*/ f_res; //运算结果	
	char offset_op, offset_num1, offset_num2;
	char c, buf[120];
	int i;	
	
	if ((strstr(char_display, "+")!= RT_NULL)|| (strstr(char_display, "-")!= RT_NULL)||(strstr(char_display, "*")!= RT_NULL)||\
		(strstr(char_display, "/")!= RT_NULL)||(strstr(char_display, "%")!= RT_NULL))
	{//有操作符
		
		if ((char_display[LEN-2]>=48)&& (char_display[LEN-2]<=57))
		{
		//操作数后有第二个操作数 开机计算
			//定位操作符位置
			if (opert == PLUS)(c='+') ;
			if (opert == MINUS) (c='-'); 
			if (opert == MULTI) (c='*'); 
			if (opert == DIV)(c='/') ;
			if (opert == PER)(c='%') ;				
			offset_op = strchr(char_display,c)-char_display;
			//定位第一个操作数开始位置
			for (i=0; i<offset_op;i++)
			{
				if(char_display[i]!=' ')
				{
					break;
				}	
			}
			offset_num1=i;
			//定位第一个操作数开始位置
			offset_num2=offset_op+1;
			
			if (offset_num1==offset_op)
			{
				f_var1=0;
			}		
			else
			{	
				memset(buf,0 ,sizeof(buf));
				memcpy(buf,&char_display[offset_num1],offset_op-offset_num1);
				f_var1 = atoi(buf);//atof
				
				
			}
			f_var2 = atoi(&char_display[offset_num2]);//atof


			if (opert == PLUS) f_res = f_var1+f_var2;
			if (opert == MINUS) f_res = f_var1-f_var2; 
			if (opert == MULTI) f_res = f_var1*f_var2;
			if (opert == DIV) f_res = f_var1/f_var2;
			if (opert == PER) f_res = ((int)(f_var1))%((int)(f_var2));	
			
			rt_kprintf("f_var1=%d\r\n", f_var1);
			rt_kprintf("f_var2=%d\r\n", f_var2);
			rt_kprintf("f_res=%d\r\n", f_res);
			
			//计算完
			memset(buf,0 ,sizeof(buf));
			sprintf(buf, "%d", f_res); //将f_res转为字符串 //%f
			
			i=strlen(buf)+1;//结果加上=的长度
			rt_kprintf("i=%d\r\n", i);
			memset(char_res,' ' ,sizeof(char_res));
			memcpy(&char_res[LEN-(LEN-offset_num1-1+i)-1], &char_display[offset_num1], LEN-offset_num1-1);
			char_res[LEN-i-1]='=';
			sprintf(&char_res[LEN-i], "%d", f_res); //将f_res转为字符串 //%f
			/*
			if (strstr(&char_res[LEN-8], ".000000")!= RT_NULL)
			{
				for (i=LEN-2; i>=7; i--)
				{//去掉浮点数后那些没用的0和.
					char_res[i]=char_res[i-7];
				}
			}	
			else
			{	//计算后面有几个没用的'0'
				int j;
				for (j=LEN-2; j>=LEN-7; j--)
				{
					if (char_res[j]!='0')
					  break;
				}
				
				for (i=LEN-2; i>=LEN-2-j; i--)
				{//去掉浮点数后那些没用的0
					char_res[i]=char_res[i-(LEN-2-j)];
				}
			}	
			*/
			char_res[LEN-1]=0;
		}
		else
		{
			memset(char_res,0 ,sizeof(char_res));
			memcpy(char_res, "哥们，操作数不全哦！", LEN-1);
				
			//复位变量
			point_flag =0;
			rt_memset(char_display, ' ', sizeof(char_display));
			char_display[LEN-1]=0;
			step = 0;	
		}	
	}
	else
	{	    
			memset(char_res,0 ,sizeof(char_res));
			memcpy(char_res, "逗我玩呢？无操作符啊", LEN-1);
			
			//复位变量
			point_flag =0;
			rt_memset(char_display, ' ', sizeof(char_display));
			char_display[LEN-1]=0;
			step = 0;		
	}	
		
}

void char_res_append(char c)
{
	int i;
	
	switch(step)
	{
		case 0: //还没第一个操作数 或已经有第一个操作数但还没操作符
			rt_kprintf("step=%d\r\n", step);
			
			if (input_type(c)==C_POINT)
			{//初始态 输入 .
				if (point_flag!=0) break; //操作数中已经有个小数点了，忽略
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;	
				point_flag =1;
				rtgui_label_set_text(label_result, char_display);
			}	
			else if (input_type(c)==C_NUM)
			{//初始态 输入 数字
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;	
				rtgui_label_set_text(label_result, char_display);				
			}	
			else
			{
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;					
				step =1;//已经有第一个操作数和操作符了
				point_flag =0;
				if (c=='+') opert = PLUS;
				if (c=='-') opert = MINUS;
				if (c=='*') opert = MULTI;
				if (c=='/') opert = DIV;
				if (c=='%') opert = PER;	
				rtgui_label_set_text(label_result, char_display);		
			}	
			
			break;
		case 1://已经有第一个操作数和操作符了
			rt_kprintf("step=%d\r\n", step);
			
			if (input_type(c)==C_POINT)
			{
				break;
			}			
			else if (input_type(c)==C_OPERT)
			{//已经有个操作符了，再来一个操作符，则用后来的操作符替换原操作符
				char_display[LEN-2] = c;					
				if (c=='+') opert = PLUS;
				if (c=='-') opert = MINUS;
				if (c=='*') opert = MULTI;
				if (c=='/') opert = DIV;
				if (c=='%') opert = PER;					
				rtgui_label_set_text(label_result, char_display);		
			}	
			else
			{
				step =2;//准备输入最后一个操作数
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;	
				rtgui_label_set_text(label_result, char_display);						
			}	
			break;
		case 2://准备输入最后一个操作数
			rt_kprintf("step=%d\r\n", step);
			
			if (input_type(c)==C_POINT)
			{//输入 .
				if (point_flag!=0) break; //操作数中已经有个小数点了，忽略
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;	
				point_flag =1;
				rtgui_label_set_text(label_result, char_display);
			}	
			else if (input_type(c)==C_NUM)
			{//初始态 输入 数字
				for(i=0; i<LEN-2; i++)
				{
					char_display[i] = char_display[i+1];
				}
				char_display[LEN-2] = c;	
				rtgui_label_set_text(label_result, char_display);				
			}	
			else//按运算符 等于是要求计算
			{
			    cal();
				rtgui_label_set_text(label_result, char_res);	
				
				//复位变量
				point_flag =0;
				rt_memset(char_display, ' ', sizeof(char_display));
				char_display[LEN-1]=0;
				step = 0;				
			}	
			
			break;	
	}
	

	rt_kprintf("%s\r\n", char_display);
	
}
/**
* @function name :win1_event_handler
* @param : *object: object pointer
* @param : *event: event pointer
* @return : rt_bool_t
* @note :the win event handle function of win1
**/
static rt_bool_t win1_event_handler(struct rtgui_object *object, struct rtgui_event *event)
{
/* add you code here */
	rt_kprintf("event->type:%d\r\n", event->type);
	return rtgui_win_event_handler(object,event);
}


/**
* @function name :button_num1_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num1
**/
static void button_num1_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('1');
}


/**
* @function name :button_plus_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_plus
**/
static void button_plus_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('+');
}


/**
* @function name :button_clr_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_clr
**/
static void button_clr_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_clr();
}


/**
* @function name :button_equal_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_equal
**/
static void button_equal_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	cal();
	rtgui_label_set_text(label_result, char_res);	
				
	//复位变量
	point_flag =0;
	rt_memset(char_display, ' ', sizeof(char_display));
	char_display[LEN-1]=0;
	step = 0;	
}


/**
* @function name :button_minus_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_minus
**/
static void button_minus_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('-');
}


/**
* @function name :button_div_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_div
**/
static void button_div_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('/');

}


/**
* @function name :button_multi_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_multi
**/
static void button_multi_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('*');
}


/**
* @function name :button_num2_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num2
**/
static void button_num2_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('2');

}


/**
* @function name :button_num3_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num3
**/
static void button_num3_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('3');
}


/**
* @function name :button_num6_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num6
**/
static void button_num6_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('6');
}


/**
* @function name :button_num5_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num5
**/
static void button_num5_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('5');
}


/**
* @function name :button_num4_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num4
**/
static void button_num4_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('4');

}


/**
* @function name :button_num9_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num9
**/
static void button_num9_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('9');

}


/**
* @function name :button_num8_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num8
**/
static void button_num8_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('8');
}


/**
* @function name :button_num7_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num7
**/
static void button_num7_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('7');
}


/**
* @function name :button_prec_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_prec
**/
static void button_prec_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('%');
}


/**
* @function name :button_point_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_point
**/
static void button_point_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('.');
}


/**
* @function name :button_num0_onbutton
* @param : *object: object pointer
* @param : *event: event pointer
* @return : void
* @note :the onbutton event function of button_num0
**/
static void button_num0_onbutton(struct rtgui_object *object, struct rtgui_event *event)
{
	/* add you code here */
	char_res_append('0');
}


/**
* @function name :win1_ui_init()
* @param : void
* @return : rtgui_win_t *
* @note :the init function of win1
**/

struct rtgui_win * win1_ui_init(struct rtgui_win *parent)
{

	struct rtgui_win *win1;
	rtgui_rect_t rect={0,0,800,480};

	struct rtgui_button *button_num1,*button_plus,*button_clr,*button_equal,*button_minus,*button_div,\
	*button_multi,*button_num2,*button_num3,*button_num6,*button_num5,*button_num4,*button_num9,*button_num8,\
	*button_num7,*button_prec,*button_point,*button_num0;


	/* win1 Create */
	{
		win1=rtgui_win_create(parent,"简易计算器-RTGUI",&rect,RTGUI_WIN_STYLE_CLOSEBOX);
		rtgui_object_set_event_handler(RTGUI_OBJECT(win1), win1_event_handler);
		RTGUI_WIDGET_BACKGROUND(win1) = RTGUI_RGB(0, 192, 192);
	}
	/* button_num1 Create */
	{
		button_num1=rtgui_button_create("1");
		RTGUI_RECT(rect,13,125,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num1), &rect);
		rtgui_button_set_onbutton(button_num1, button_num1_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num1)); 
	}
	/* button_plus Create */
	{
		button_plus=rtgui_button_create("+");
		RTGUI_RECT(rect,598,125,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_plus), &rect);
		rtgui_button_set_onbutton(button_plus, button_plus_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_plus)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_plus)) = RTGUI_RGB(0, 192, 0);
	}
	/* button_clr Create */
	{
		button_clr=rtgui_button_create("清除");
		RTGUI_RECT(rect,598,368,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_clr), &rect);
		rtgui_button_set_onbutton(button_clr, button_clr_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_clr)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_clr)) = RTGUI_RGB(128, 64, 64);
	}
	/* button_equal Create */
	{
		button_equal=rtgui_button_create("=");
		RTGUI_RECT(rect,694,368,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_equal), &rect);
		rtgui_button_set_onbutton(button_equal, button_equal_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_equal)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_equal)) = RTGUI_RGB(255, 0, 0);
	}
	/* label_result Create */
	{
		label_result=rtgui_label_create("");
		RTGUI_RECT(rect,12,69,763,50);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);
		rtgui_widget_set_rect(RTGUI_WIDGET(label_result), &rect);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(label_result)); 
		RTGUI_WIDGET_FOREGROUND(RTGUI_WIDGET(label_result)) = RTGUI_RGB(255, 0, 0);
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(label_result)) = RTGUI_RGB(255, 255, 255);
		//RTGUI_WIDGET_ALIGN(label_result)=RTGUI_ALIGN_CENTER_VERTICAL|RTGUI_ALIGN_RIGHT;
		
	}
	/* button_minus Create */
	{
		button_minus=rtgui_button_create("-");
		RTGUI_RECT(rect,694,125,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_minus), &rect);
		rtgui_button_set_onbutton(button_minus, button_minus_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_minus)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_minus)) = RTGUI_RGB(0, 192, 0);
	}
	/* button_div Create */
	{
		button_div=rtgui_button_create("/");
		RTGUI_RECT(rect,694,241,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_div), &rect);
		rtgui_button_set_onbutton(button_div, button_div_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_div)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_div)) = RTGUI_RGB(0, 192, 0);
	}
	/* button_multi Create */
	{
		button_multi=rtgui_button_create("*");
		RTGUI_RECT(rect,598,241,81,91);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_multi), &rect);
		rtgui_button_set_onbutton(button_multi, button_multi_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_multi)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_multi)) = RTGUI_RGB(0, 192, 0);
	}
	/* button_num2 Create */
	{
		button_num2=rtgui_button_create("2");
		RTGUI_RECT(rect,204,125,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num2), &rect);
		rtgui_button_set_onbutton(button_num2, button_num2_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num2)); 
	}
	/* button_num3 Create */
	{
		button_num3=rtgui_button_create("3");
		RTGUI_RECT(rect,397,125,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num3), &rect);
		rtgui_button_set_onbutton(button_num3, button_num3_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num3)); 
	}
	/* button_num6 Create */
	{
		button_num6=rtgui_button_create("6");
		RTGUI_RECT(rect,397,215,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num6), &rect);
		rtgui_button_set_onbutton(button_num6, button_num6_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num6)); 
	}
	/* button_num5 Create */
	{
		button_num5=rtgui_button_create("5");
		RTGUI_RECT(rect,204,215,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num5), &rect);
		rtgui_button_set_onbutton(button_num5, button_num5_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num5)); 
	}
	/* button_num4 Create */
	{
		button_num4=rtgui_button_create("4");
		RTGUI_RECT(rect,13,215,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num4), &rect);
		rtgui_button_set_onbutton(button_num4, button_num4_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num4)); 
	}
	/* button_num9 Create */
	{
		button_num9=rtgui_button_create("9");
		RTGUI_RECT(rect,397,306,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num9), &rect);
		rtgui_button_set_onbutton(button_num9, button_num9_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num9)); 
	}
	/* button_num8 Create */
	{
		button_num8=rtgui_button_create("8");
		RTGUI_RECT(rect,204,306,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num8), &rect);
		rtgui_button_set_onbutton(button_num8, button_num8_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num8)); 
	}
	/* button_num7 Create */
	{
		button_num7=rtgui_button_create("7");
		RTGUI_RECT(rect,13,306,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num7), &rect);
		rtgui_button_set_onbutton(button_num7, button_num7_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num7)); 
	}
	/* button_prec Create */
	{
		button_prec=rtgui_button_create("%");
		RTGUI_RECT(rect,397,401,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_prec), &rect);
		rtgui_button_set_onbutton(button_prec, button_prec_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_prec)); 
		RTGUI_WIDGET_BACKGROUND(RTGUI_WIDGET(button_prec)) = RTGUI_RGB(0, 192, 0);
	}
	/* button_point Create */
	{
		button_point=rtgui_button_create(".");
		RTGUI_RECT(rect,204,401,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_point), &rect);
		rtgui_button_set_onbutton(button_point, button_point_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_point)); 
	}
	/* button_num0 Create */
	{
		button_num0=rtgui_button_create("0");
		RTGUI_RECT(rect,13,401,159,69);
		rtgui_widget_rect_to_device(RTGUI_WIDGET(win1),&rect);rtgui_widget_set_rect(RTGUI_WIDGET(button_num0), &rect);
		rtgui_button_set_onbutton(button_num0, button_num0_onbutton);
		rtgui_container_add_child(RTGUI_CONTAINER(win1), RTGUI_WIDGET(button_num0)); 
	}
	
	char_res_clr();
	
	return win1;
}


void main(void)
{
    struct rtgui_app* application;
    struct rtgui_win* win;

    application = rtgui_app_create("calculator");
    if (application != RT_NULL)
    {
        win = win1_ui_init(RT_NULL);
	}
	
	 rtgui_win_show(win, RT_TRUE);

     rtgui_app_run(application);
     rtgui_app_destroy(application);

}
