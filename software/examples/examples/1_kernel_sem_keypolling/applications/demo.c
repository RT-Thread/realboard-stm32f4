/*
  此demo用于演示信号量的使用
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif

#include <board.h>

#define KEY_PORT  GPIOF
#define KEY_PIN   (GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |\
                     GPIO_Pin_10| GPIO_Pin_11)
#define KEY_CLCOK RCC_AHB1Periph_GPIOF

static void key_gpio_config(void)
{
	//gpio init
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIOA GPIOB Periph clock enable */
    RCC_AHB1PeriphClockCmd(KEY_CLCOK , ENABLE);

    /* Configure Pin in input pushpull mode */
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	#if LCD_VERSION!=1	//魔笛f4 使用上拉
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	
	#endif
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;

static int key;
static struct rt_semaphore sem;

static void rt_thread_entry1(void* parameter)
{
    int temp;

	key_gpio_config();
    
    while (1)
    {
        key = GPIO_ReadInputData(KEY_PORT);
 		#if LCD_VERSION==1
		#else 
			key =~key;
			key &= (0|KEY_PIN);
		#endif

        if (key & KEY_PIN)
        {
            temp = key;
            rt_thread_delay(RT_TICK_PER_SECOND / 50);
        	key = GPIO_ReadInputData(KEY_PORT);
 			#if LCD_VERSION==1
			#else 
				key =~key;
			    key &= (0|KEY_PIN);
			#endif
            if (key == temp)
                rt_sem_release(&sem);
        }
        rt_thread_delay(RT_TICK_PER_SECOND/10);	
   }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;

static int key;
static void rt_thread_entry2(void* parameter)
{
    while (1)
    {
        rt_sem_take(&sem, RT_WAITING_FOREVER);

        if (key & KEY_PIN)
        {
            rt_kprintf("some keys has been pressed : %X\n", key);
        }
    }
}

int demo_init(void)
{
    rt_err_t result;
    
    result = rt_sem_init(&sem, "sem", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("error, init sem failed!\n");
        return 0;
    }
    
    rt_thread_init(&thread1,
                   "keyp", //producer
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),11,25);
    rt_thread_startup(&thread1);
    
    rt_thread_init(&thread2,
                   "keyc", //consumer
                   rt_thread_entry2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),11,24);
    rt_thread_startup(&thread2);
    return 0;

}
