/*
	此demo用于演示优先级反转
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


static rt_sem_t sem;
static rt_uint8_t t1_count, t2_count,worker_count;
static rt_thread_t t1, t2, worker;

static void thread1_entry(void* parameter)
    {
    rt_err_t result;
    
    result = rt_sem_take(sem, RT_WAITING_FOREVER);
    
    for(t1_count = 0; t1_count < 10; t1_count ++)
    {
        rt_kprintf("thread1: got semaphore, count: %d\n", t1_count);
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
    
    rt_kprintf("thread1: release semaphore\n");
    rt_sem_release(sem);
}

static void thread2_entry(void* parameter)
{
    rt_err_t result;

    while (1)
    {
        result = rt_sem_take(sem, RT_WAITING_FOREVER);
        rt_kprintf("thread2: got semaphore\n");
        if (result != RT_EOK)
        {
            return;
        }
        rt_kprintf("thread2: release semaphore\n");
        rt_sem_release(sem);
        
        rt_thread_delay(5);
        result = rt_sem_take(sem, RT_WAITING_FOREVER);
        t2_count ++;
        rt_kprintf("thread2: got semaphore, count: %d\n", t2_count);
    }
}

static void worker_thread_entry(void* parameter)
{
    rt_thread_delay(5);
    for(worker_count = 0; worker_count < 10; worker_count++)
    {
        rt_kprintf("worker:  count: %d\n", worker_count);
    }
    rt_thread_delay(RT_TICK_PER_SECOND);
}


int demo_init(void)
{
    sem = rt_sem_create("sem", 1, RT_IPC_FLAG_PRIO);
    if (sem == RT_NULL)
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
