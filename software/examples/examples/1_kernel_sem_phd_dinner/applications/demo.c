/*
 * 程序清单：哲学家就餐问题
 * 参考《Operating Systems Design and Implementation》 By Andrew S. Tanenbaum
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif

#define THREAD_STACK_SIZE 1024  /* 定义线程堆栈的大小 */
#define N 5                     /* 定义哲学家的数目5 */
struct rt_semaphore sem[N];     /* 每位哲学家一个信号量 */
struct rt_semaphore sem_lock;   /* 定义二值信号量实现临界区互斥 */

enum _phd_state {               /* 定义使用枚举类型表示哲学家状态*/
THINKING = 0,
HUNGRY, 
EATING,
} phd_state[N];                 /* 定义哲学家状态数组 */

const char * status_string[N] = 
{
    "thinking",
    "hungry",
    "eating",
};

#define LEFT_PHD(i)   ((i+N-1)%N) /* 哲学家i左边的哲学家 */
#define RIGHT_PHD(i)  ((i+1)%N)   /* 哲学家i右边的哲学家 */

static void put_forks(int i);
static void take_forks(int i);
static void test(int i);

/* 哲学家线程 */
void phd_thread_entry(void* parameter)
{
    int i;

    i = (int)parameter;
    rt_kprintf("phd %i starts...\n", i);
    while(1)
    {
        /* thinking */    
        rt_thread_delay(RT_TICK_PER_SECOND);
        rt_kprintf("phd %d is %s\n", i, status_string[phd_state[i]]);    

        /* take forks */
        take_forks(i);

        /* eating */
        rt_kprintf("phd %d is %s\n", i, status_string[phd_state[i]]);    
        rt_thread_delay(RT_TICK_PER_SECOND*2);

        /* put forks */
        put_forks(i);
    }
}

void take_forks(int i)
{
    /* 进入临界区*/
    rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
    phd_state[i] = HUNGRY;
    test(i);
    /* 退出临界区*/
    rt_sem_release(&sem_lock);

    /* 如果不处于EATING状态则阻塞哲学家 */
    rt_sem_take(&sem[i], RT_WAITING_FOREVER);
}

void put_forks(int i)
{
    /* 进入临界区*/
    rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
    phd_state[i] = THINKING;

    test(LEFT_PHD(i));
    test(RIGHT_PHD(i));
    /* 退出临界区*/
    rt_sem_release(&sem_lock);
}

void test(int i)
{    
    if (phd_state[i] == HUNGRY &&
        phd_state[LEFT_PHD(i)] != EATING &&
        phd_state[RIGHT_PHD(i)] != EATING)
    {
        phd_state[i] = EATING;

        /* 可以得到叉子，故发布信号量 */
        rt_sem_release(&sem[i]);
    }
}


int demo_init(void)
{
    int i;
    rt_thread_t tid;
    rt_err_t result;

    /* 初始化信号量 */
    result = rt_sem_init(&sem_lock , "lock", 1, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
        goto _error;
    for (i=0; i<5; i++)
    {
        result = rt_sem_init(&sem[i] , "sem", 0, RT_IPC_FLAG_FIFO);
        if (result != RT_EOK)
            goto _error;
    }


    for (i=0; i<5; i++)
    {
        tid = rt_thread_create(
                       "phd",
                       phd_thread_entry,
                       (void *)i,
                       THREAD_STACK_SIZE, 10, RT_TICK_PER_SECOND*3);
        if(tid != RT_NULL)
            rt_thread_startup(tid);
    }

    return 0;
_error:
    rt_kprintf("init semaphore failed.\n");
    return -1;
}
