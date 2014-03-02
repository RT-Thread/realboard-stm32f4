/*
 * 程序清单：生产者消费者例子
 *
 * 这个例子中将创建两个线程用于实现生产者消费者问题
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* 定义线程堆栈的大小 */
#define THREAD_STACK_SIZE 1024
/* 定义最大5个元素能够被产生 */
#define MAXSEM 5

/* 用于放置生产的整数数组 */
rt_uint32_t array[MAXSEM];
/* 指向生产者、消费者在array数组中的读写位置 */
static rt_uint32_t set, get;

struct rt_semaphore sem_lock;
struct rt_semaphore sem_empty, sem_full;

/* 生产者线程入口 */
void producer_thread_entry(void* parameter)
{
    int cnt = 0;

    /* 运行20次 */
    while (cnt < 20)
    {
        /* 获取一个空位 */
        rt_sem_take(&sem_empty, RT_WAITING_FOREVER);

        /* 修改array内容，上锁 */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        array[set%MAXSEM] = cnt + 1;
        rt_kprintf("the producer generates a number: %d\n", array[set%MAXSEM]);
        set++;
        rt_sem_release(&sem_lock);

        /* 发布一个满位 */
        rt_sem_release(&sem_full);
        cnt++;

        /* 暂停一段时间 */
        //rt_thread_delay(15);
    }

    rt_kprintf("the producer exit!\n");
}

/* 消费者线程入口 */
void consumer_thread_entry(void* parameter)
{
    rt_uint32_t no;
    rt_uint32_t sum;

    /* 第n个线程，由入口参数传进来 */
    no = (rt_uint32_t)parameter;
    sum = 0;

    while(1)
    {
        /* 获取一个满位 */
        rt_sem_take(&sem_full, RT_WAITING_FOREVER);

        /* 临界区，上锁进行操作 */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        sum += array[get%MAXSEM];
        rt_kprintf("the consumer[%d] get a number: %d\n", no, array[get%MAXSEM] );
        get++;
        rt_sem_release(&sem_lock);

        /* 释放一个空位 */
        rt_sem_release(&sem_empty);

        /* 生产者生产到20个数目，停止，消费者线程相应停止 */
        if (get == 20) 
            break;

        /* 暂停一小会时间 */
        rt_thread_delay(10);
    }

    rt_kprintf("the consumer[%d] exits, sum is %d \n ", no, sum);
}

int demo_init(void)
{
    rt_thread_t tid;
    rt_err_t result;

    /* 初始化3个信号量 */
    result = rt_sem_init(&sem_lock , "lock", 1, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
        goto _error;
    result = rt_sem_init(&sem_empty, "empty", MAXSEM, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
        goto _error;
    result = rt_sem_init(&sem_full , "full", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
        goto _error;

    tid = rt_thread_create(
                   "producer",
                   producer_thread_entry,
                   RT_NULL,
                   THREAD_STACK_SIZE, 10, 5);
    if(tid != RT_NULL)
        rt_thread_startup(tid);

    tid = rt_thread_create(
                   "consumer1",
                   consumer_thread_entry,
                   (void *)1,
                   THREAD_STACK_SIZE, 11, 2);
    if(tid != RT_NULL)
        rt_thread_startup(tid);

    tid = rt_thread_create(
                   "consumer2",
                   consumer_thread_entry,
                   (void *)2,
                   THREAD_STACK_SIZE, 11, 2);
    if(tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;

_error:
    rt_kprintf("init semaphore failed.\n");
    return -1;
}
