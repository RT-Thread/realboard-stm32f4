/*
	rewrite keyboard driver  2013-11-11 by jiezhi320
*/

#include <rtthread.h>
#include <board.h>

#include <rtgui/event.h>
#include <rtgui/rtgui_server.h>

//���水������ʹ��
#define DEBOUNCE_SHORT_TIME 	3   // �ᴥ��������ʱ��5����λ��20���룩
#define DEBOUNCE_LONG_TIME  	5  // ������ʱ��DEBOUNCE_COUT_FIRST+DEBOUNCE_COUT_INTERVAL*DEBOUNCE_LONG_TIME����λ��10���룩
#define DEBOUNCE_COUT_FIRST 	500//50 // ���������ʱ��100����λ��20���룩
#define DEBOUNCE_COUT_INTERVAL 	10  // ���������ʱ��50����λ��20���룩

//���ⰴ������ʹ��
#define C_RELASE_COUT			3
#define C_SHORT_COUT			3	//3*20ms
#define C_SPECIAL_LONG_COUT		60  //60*20ms

//������־
#define C_FLAG_SHORT			0x00000001
#define C_FLAG_COUNT			0x00000002
#define C_FLAG_LONG				0x00000004
#define C_FLAG_RELASE			0x00000008

//������ֵ
#define  C_UP_KEY 				0x1
#define  C_DOWN_KEY 			0x2
#define  C_LEFT_KEY 			0x4
#define  C_RIGHT_KEY 			0x8
#define  C_STOP_KEY 			0x10
#define  C_MENU_KEY 			0x20
#define  C_ENTER_KEY 			0x40
#define  C_SPECIAL_KEY 			C_ENTER_KEY

/*enter������ ���Ƕ���Ϊhome��*/
#define  C_HOME_KEY 			C_SPECIAL_KEY+0x44

/*
KEY_UP    PF6
KEY_DOWN  PF7
KEY_LEFT  PF8
KEY_RIGHT PF9
KEY_STOP  PF10
KEY_MENU  PF11
KEY_ENTER PA0  (WKUP)
*/
#define key_up_GETVALUE()     GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6)
#define key_down_GETVALUE()   GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)
#define key_left_GETVALUE()   GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_8)
#define key_right_GETVALUE()  GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_9)
#define key_stop_GETVALUE()   GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_10)
#define key_menu_GETVALUE()   GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_11)
#define key_enter_GETVALUE()  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

/* ʹ���������ķ�ʽ�����������õ�������Ԫ�ؽ��д�� */
struct rtgui_key
{
    rt_timer_t poll_timer;
    struct rtgui_event_kbd kbd_event;
	
	rt_uint32_t key_last;
	rt_uint32_t key_current;
	//��⵽�İ���ֵ
	rt_uint32_t key_get;
	//���水��ʹ��
	rt_uint32_t key_debounce_count;
	rt_uint32_t key_long_count;
	//���ⰴ��ʹ��
	rt_uint32_t key_special_count;	
	rt_uint32_t key_relase_count;
	//������־
	rt_uint32_t key_flag;
	
};

static struct rtgui_key *key;


static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* init gpio configuration */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOF,
                           ENABLE);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	#if LCD_VERSION!=1	//ħ��f4 ʹ������
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
	#endif
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
                                    | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
}




static void key_timeout(void* parameter)
{
	key->key_current = key_up_GETVALUE();
	key->key_current |= key_down_GETVALUE()<<1;	
	key->key_current |= key_left_GETVALUE()<<2;	
	key->key_current |= key_right_GETVALUE()<<3;	
	key->key_current |= key_stop_GETVALUE()<<4;	
	key->key_current |= key_menu_GETVALUE()<<5;	
	key->key_current |= key_enter_GETVALUE()<<6;		
	#if LCD_VERSION==1


	#else 
	key->key_current=~(key->key_current);
	key->key_current&=0x0000007f;
	
	#endif

	key->key_flag &= ~C_FLAG_SHORT;
	key->key_flag &= ~C_FLAG_COUNT;
	key->key_flag &= ~C_FLAG_LONG;
	key->key_get = 0;	

//
	/*�����г����Ͷ̰��������������*/	
	if ((key->key_flag)&C_FLAG_RELASE)
	{//���ż�
		if (key->key_current == 0)
		{
			if ((++(key->key_relase_count)) >= C_RELASE_COUT)
			{ //�����Ѿ��ſ�
				key->key_relase_count = 0;
				key->key_flag &= ~C_FLAG_RELASE;
			}
		}
		else
		{
			key->key_relase_count = 0;
		}
	}
	else
	{//��鰴��
		if (key->key_current == C_SPECIAL_KEY)		
		{
			if ((++(key->key_special_count)) >= C_SPECIAL_LONG_COUT)
			{
				key->key_special_count = 0;
				
				key->key_get = C_HOME_KEY;      
				key->key_flag |= C_FLAG_LONG;	//����� ����������
				key->key_flag |= C_FLAG_RELASE;;//���º�Ҫ����ż�
			}
		}
		else
		{//�ſ�����ż��̰�
			if ((key->key_special_count >= C_SHORT_COUT) && (key->key_special_count <C_SHORT_COUT+30)) 
			{
				key->key_get = C_SPECIAL_KEY;
				key->key_flag |= C_FLAG_SHORT;	//����� �̰�������
			}
			key->key_special_count = 0;
		}
	}
	
// ��ͨ��������
	if((key->key_current == 0)||(key->key_current != key->key_last)|| (key->key_current == C_SPECIAL_KEY))
	{
		key->key_debounce_count = 0;	//��һ��	
		key->key_long_count=0;	        //���������������
	}
	else
	{
		if(++(key->key_debounce_count) == DEBOUNCE_SHORT_TIME)
		{
			key->key_get = key->key_current;
			key->key_flag |= C_FLAG_SHORT;	//�̰�������
		}
		if(key->key_debounce_count == DEBOUNCE_COUT_FIRST + DEBOUNCE_COUT_INTERVAL)
		{
			key->key_get = key->key_current;
			key->key_flag |= C_FLAG_COUNT;	//������ ��������
			key->key_debounce_count = DEBOUNCE_COUT_FIRST;
			++(key->key_long_count);			
		}
	
		if(key->key_long_count == DEBOUNCE_LONG_TIME)
		{
			key->key_get = key->key_current;
			key->key_flag |= C_FLAG_LONG;	//�̰�������
			key->key_long_count=DEBOUNCE_LONG_TIME+1;
		}		
	}
	
	key->key_last = key->key_current;				// ���汾�μ�ֵ
	
	
	/* ��⵽��������ϵͳ�ϱ���ֵ */
	key->kbd_event.key = RTGUIK_UNKNOWN;
    key->kbd_event.type = RTGUI_KEYDOWN;
	
	if (key->key_get)
	{	
		rt_kprintf("key = %x \n",key->key_get);
		if (((key->key_get)==C_UP_KEY) && ((key->key_flag) & C_FLAG_SHORT))
			key->kbd_event.key  = RTGUIK_UP;
		
		if (((key->key_get)==C_DOWN_KEY) && ((key->key_flag) & C_FLAG_SHORT))
			key->kbd_event.key  = RTGUIK_DOWN;
		
		if (((key->key_get)==C_LEFT_KEY) && ((key->key_flag) & C_FLAG_SHORT))
			key->kbd_event.key  = RTGUIK_LEFT;
		
		if (((key->key_get)==C_RIGHT_KEY) && ((key->key_flag) & C_FLAG_SHORT))
			key->kbd_event.key  = RTGUIK_RIGHT;
		
		//if (((key->key_get)==C_STOP_KEY) && ((key->key_flag) & C_FLAG_SHORT))
		//	key->kbd_event.key  = RTGUIK_UP;
		//if (((key->key_get)==C_MENU_KEY) && ((key->key_flag) & C_FLAG_SHORT))
		//	key->kbd_event.key  = RTGUIK_UP; 
		if ((key->key_get)==C_ENTER_KEY) 
			key->kbd_event.key  = RTGUIK_RETURN;		
		
		if ((key->key_get)==C_HOME_KEY) 
			key->kbd_event.key  = RTGUIK_HOME;	
	}	
	
	if (key->kbd_event.key != RTGUIK_UNKNOWN)
    {
        /* ���ϱ���������*/
        rtgui_server_post_event(&(key->kbd_event.parent), sizeof(key->kbd_event));

        /* delay to post up event */
        rt_thread_delay(2);

        /* ���ϱ������ɿ������һ���Ӱ��µ��ɿ������*/
        key->kbd_event.type = RTGUI_KEYUP;
        rtgui_server_post_event(&(key->kbd_event.parent), sizeof(key->kbd_event));
    }
}


int rt_hw_key_init(void)
{
	GPIO_Configuration();
	
	key = (struct rtgui_key*)rt_malloc (sizeof(struct rtgui_key));
    if (key == RT_NULL)
		return -1; /* no memory yet */
	
	/* init keyboard event */
    RTGUI_EVENT_KBD_INIT(&(key->kbd_event));
	key->kbd_event.wid = RT_NULL;
    key->kbd_event.mod  = RTGUI_KMOD_NONE;
    key->kbd_event.unicode = 0;
	
	key->key_last = 0;
	key->key_current = 0;
	key->key_get = 0;
	key->key_debounce_count = 0;
	key->key_long_count = 0;
	key->key_special_count = 0;
	key->key_relase_count = 0;
	key->key_flag = 0;
	
	/* create 1/50=20ms  timer */
    key->poll_timer = rt_timer_create("key", key_timeout, RT_NULL,
                                        RT_TICK_PER_SECOND/50, RT_TIMER_FLAG_PERIODIC);
	
     /* ������ʱ�� */
    if (key->poll_timer != RT_NULL)
        rt_timer_start(key->poll_timer); 	
	
	return 0;
}

//INIT_DEVICE_EXPORT(rt_hw_key_init);
