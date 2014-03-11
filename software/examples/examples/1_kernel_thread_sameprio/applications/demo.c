/*
	此demo用于演示相同优先级的轮询调度
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


static struct rt_thread thread1;
static struct rt_thread thread2;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[512];

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[512];

static void thread1_entry(void* parameter)
{
    rt_uint8_t i;
    for(i = 0; i < 6; i ++)
    {
        rt_kprintf("Thread1:\n\r");
        rt_kprintf("This is \n");
        rt_kprintf("a\n");
        rt_kprintf("demo\n");
        rt_thread_delay(10);
    }
}

static void thread2_entry(void* parameter)
{
    rt_uint8_t j;
    for(j = 0; j <60; j ++)
    {
        rt_kprintf("Thread2:\n\r");
        rt_kprintf("This is \n");
        rt_kprintf("a\n");
        rt_kprintf("demo\n");
    }
}


int demo_init(void)
{
    rt_err_t result;

    result = rt_thread_init(&thread1,
        "t1",
        thread1_entry, RT_NULL,
        &thread1_stack[0], sizeof(thread1_stack),
        6, 10);
    if (result == RT_EOK)
        rt_thread_startup(&thread1);


    result = rt_thread_init(&thread2,
        "t2",
        thread2_entry, RT_NULL,
        &thread2_stack[0], sizeof(thread2_stack),
        6, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread2);


    return 0;
}
