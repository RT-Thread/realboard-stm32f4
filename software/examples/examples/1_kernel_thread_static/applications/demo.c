/*
	此demo用于演示静态线程
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* 线程1控制块 */
static struct rt_thread thread1;
/* 线程1栈 */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread1_stack[512];

/* 线程2控制块 */
static struct rt_thread thread2;
/* 线程2栈 */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread2_stack[512];

/* 线程1入口 */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
        /* 线程1采用低优先级运行，一直打印计数值 */
        rt_kprintf("thread count: %d\n", count ++);
        /* 线程1延时1秒 */
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

/* 线程2入口 */
static void thread2_entry(void *parameter)
{
    /* 线程2拥有较高的优先级，以抢占线程1而获得执行 */

    /* 线程2启动后先睡眠10秒 */
    rt_thread_delay(RT_TICK_PER_SECOND * 10);

    /*
     * 线程2唤醒后直接执行线程1脱离，线程1将从就绪线程队列中删除
     */
    rt_thread_detach(&thread1);

    /*
     * 线程2继续休眠10个OS Tick然后退出
     */
    rt_thread_delay(10);
}


int demo_init(void)
{
    rt_err_t result;

    /* 初始化线程1 */
    result = rt_thread_init(&thread1, "t1", /* 线程名：t1 */
        thread1_entry, RT_NULL, /* 线程的入口是thread1_entry，入口参数是RT_NULL*/
        &thread1_stack[0], sizeof(thread1_stack), /* 线程栈是thread1_stack */
        7, 10);
    if (result == RT_EOK) /* 如果返回正确，启动线程1 */
        rt_thread_startup(&thread1);

    /* 初始化线程2 */
    result = rt_thread_init(&thread2, "t2", /* 线程名：t2 */
        thread2_entry, RT_NULL, /* 线程的入口是thread2_entry，入口参数是RT_NULL*/
        &thread2_stack[0], sizeof(thread2_stack), /* 线程栈是thread2_stack */
        6, 10);
    if (result == RT_EOK) /* 如果返回正确，启动线程2 */
        rt_thread_startup(&thread2);

    return 0;
}
