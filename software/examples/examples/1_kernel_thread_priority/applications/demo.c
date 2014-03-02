/*
	此demo用于演示优先级抢占
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


struct rt_thread thread1;
struct rt_thread thread2;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[512];

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[512];
static rt_uint32_t count = 0;

/*
 * the priority of thread1 > the priority of thread2
 */
static void thread1_entry(void* parameter)
{
    for(;count<5;count++)
    {
        rt_thread_delay(3*RT_TICK_PER_SECOND);
        rt_kprintf("count = %d\n", count);
    }
}

static void thread2_entry(void* parameter)
{
    rt_tick_t tick;
    rt_uint32_t i;

    for(i=0; i<15; i++)
    {
        tick = rt_tick_get();
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("tick = %d\n",tick);
    }
}


int demo_init(void)
{
    rt_err_t result;

    result = rt_thread_init(&thread1,
        "t1",
        thread1_entry, RT_NULL,
        &thread1_stack[0], sizeof(thread1_stack),
        5, 5);
    
    if (result == RT_EOK)
        rt_thread_startup(&thread1);
    
    rt_thread_init(&thread2,
        "t2",
        thread2_entry, RT_NULL,
        &thread2_stack[0], sizeof(thread2_stack),
        7, 5);

    if (result == RT_EOK)
        rt_thread_startup(&thread2);

    return 0;
}
