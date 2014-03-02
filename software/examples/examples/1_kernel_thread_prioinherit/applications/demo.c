/*
	此demo用于演示优先级继承
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


static rt_mutex_t mutex = RT_NULL;
static rt_uint8_t t1_count, t2_count,worker_count;
static rt_thread_t t1, t2, worker;

static void thread1_entry(void* parameter)
{
    rt_err_t result;

    
    result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
    result = rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread1: got mutex\n");

    if (result != RT_EOK)
    {
        return;
    }
        
    for(t1_count = 0; t1_count < 5;t1_count ++)
    {
        rt_kprintf("thread1:count: %d\n", t1_count);
    }
    if (t2->current_priority != t1->current_priority)
    {
        rt_kprintf("thread1: released mutex\n");
        rt_mutex_release(mutex);
        rt_mutex_release(mutex);
    }
        
}

static void thread2_entry(void* parameter)
{
    rt_thread_delay(5);

    rt_mutex_take(mutex, RT_WAITING_FOREVER);
    rt_kprintf("thread2: got mutex\n");
    for(t2_count = 0; t2_count < 5;t2_count ++)
    {
        rt_kprintf("thread2: count: %d\n", t2_count);
    }
}

static void worker_thread_entry(void* parameter)
{
    rt_thread_delay(5);
    for(worker_count = 0; worker_count < 5; worker_count++)
    {
        rt_kprintf("worker:count: %d\n", worker_count);
        rt_thread_delay(5);
    }
}


int demo_init(void)
{
    mutex = rt_mutex_create("mutex", RT_IPC_FLAG_FIFO);
    if (mutex == RT_NULL)
    {
        return 0;
    }

    t1_count = t2_count = 0;

    t1 = rt_thread_create("t1",
        thread1_entry, RT_NULL,
        512, 7, 10);
    if (t1 != RT_NULL)
        rt_thread_startup(t1);
    

    t2 = rt_thread_create("t2",
        thread2_entry, RT_NULL,
        512, 5, 10);
    if (t2 != RT_NULL)
        rt_thread_startup(t2);
        
    worker = rt_thread_create("worker",
        worker_thread_entry, RT_NULL,
        512, 6, 10);
    if (worker != RT_NULL)
        rt_thread_startup(worker);

    return 0;
}
