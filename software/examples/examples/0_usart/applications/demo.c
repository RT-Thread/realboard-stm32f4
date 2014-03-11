/*
  这份demo用于演示内核串口打印和线程调度
*/
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;

static void rt_thread_entry1(void* parameter)
{

    while (1)
    {
		rt_kprintf("thread1 run...\n");
        /* sleep 1 second and switch to other thread */
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}


ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;

static void rt_thread_entry2(void* parameter)
{
    while (1)
    {
	    rt_kprintf("thread2 run...\n");
        /* sleep 0.5 second and switch to other thread */
        rt_thread_delay(RT_TICK_PER_SECOND/2);
    }
}

int demo_init(void)
{
    //------- init demo1 thread
    rt_thread_init(&thread1,
                   "demo_thr1",
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),11,5);
    rt_thread_startup(&thread1);

    //------- init demo2 thread
    rt_thread_init(&thread2,
                   "demo_thr2",
                   rt_thread_entry2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),11,5);
    rt_thread_startup(&thread2);

	return 0;
}
