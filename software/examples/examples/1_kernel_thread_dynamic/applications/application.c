#include <rtthread.h>

rt_thread_t tid1 = RT_NULL;
rt_thread_t tid2 = RT_NULL;

static void thread1_entry(void* parameter)
{
    rt_uint32_t count = 0;
    rt_kprintf("thread1 dynamicly created ok\n");
    while(1)
    {
        rt_kprintf("thread1 count: %d\n",count++);
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

static void thread2_entry(void* parameter)
{
    rt_kprintf("thread2 dynamicly created ok\n");

    rt_thread_delay(RT_TICK_PER_SECOND * 4);
    
    rt_thread_delete(tid1);
    rt_kprintf("thread1 deleted ok\n");
}

int rt_application_init()
{
    rt_thread_t init_thread;
    
    rt_err_t result;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    tid1 = rt_thread_create("thread1",
        thread1_entry, 
        RT_NULL,
        512, 6, 10);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    tid2 = rt_thread_create("thread2",
        thread2_entry, RT_NULL,
        512, 6, 10);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    return 0;
}

/*@}*/
