/*
  ��demo������ʾ��������ʹ��
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* ���������ƿ� */
static struct rt_mutex static_mutex;
/* ָ�򻥳�����ָ�� */
static rt_mutex_t dynamic_mutex = RT_NULL;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;
static void rt_thread_entry1(void* parameter)
{
    rt_err_t result;
    rt_tick_t tick;

/* 1. staic mutex demo */

    /* ��ͼ���л����������ȴ�10��OS Tick�󷵻� */
    rt_kprintf("thread1 try to get static mutex, wait 10 ticks.\n");

    /* ��õ�ǰ��OS Tick */
    tick = rt_tick_get();
    result = rt_mutex_take(&static_mutex, 10);

    if (result == -RT_ETIMEOUT)
    {
        /* ��ʱ���ж��Ƿ�պ���10��OS Tick */
        if (rt_tick_get() - tick != 10)
        {
            rt_mutex_detach(&static_mutex);
            return;
        }
        rt_kprintf("thread1 take static mutex timeout\n");
    }
    else
    {
        /* �߳�2���л������������൱����ʱ���Ż��ͷŻ�������
         * ���10��tick���߳�1�����ܻ�� */
        rt_kprintf("thread1 take a static mutex, failed.\n");
        rt_mutex_detach(&static_mutex);
        return;
    }

    /* ���õȴ���ʽ���л����� */
    rt_kprintf("thread1 try to get static mutex, wait forever.\n");
    result = rt_mutex_take(&static_mutex, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        /* ���ɹ������ʧ�� */
        rt_kprintf("thread1 take a static mutex, failed.\n");
        rt_mutex_detach(&static_mutex);
        return;
    }

    rt_kprintf("thread1 take a staic mutex, done.\n");

    /* ���뻥�������� */
    rt_mutex_detach(&static_mutex);

/* 2. dynamic mutex test */

    /* ��ͼ���л����������ȴ�10��OS Tick�󷵻� */
    rt_kprintf("thread1 try to get dynamic mutex, wait 10 ticks.\n");

    tick = rt_tick_get();
    result = rt_mutex_take(dynamic_mutex, 10);
    if (result == -RT_ETIMEOUT)
    {
        /* ��ʱ���ж��Ƿ�պ���10��OS Tick */
        if (rt_tick_get() - tick != 10)
        {
            rt_mutex_delete(dynamic_mutex);
            return;
        }
        rt_kprintf("thread1 take dynamic mutex timeout\n");
    }
    else
    {
        /* �߳�2���л������������൱����ʱ���Ż��ͷŻ�������
         * ���10��tick���߳�1�����ܻ�� */
        rt_kprintf("thread1 take a dynamic mutex, failed.\n");
        rt_mutex_delete(dynamic_mutex);
        return;
    }

    /* ���õȴ���ʽ���л����� */
    rt_kprintf("thread1 try to get dynamic mutex, wait forever.\n");
    result = rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        /* ���ɹ������ʧ�� */
        rt_kprintf("thread1 take a dynamic mutex, failed.\n");
        rt_mutex_delete(dynamic_mutex);
        return;
    }

    rt_kprintf("thread1 take a dynamic mutex, done.\n");
    /* ɾ������������ */
    rt_mutex_delete(dynamic_mutex);
}


ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;
static void rt_thread_entry2(void* parameter)
{
    rt_err_t result;
    rt_tick_t tick;

    /* 1. static mutex test */
    rt_kprintf("thread2 try to get static mutex\n");
    rt_mutex_take(&static_mutex, 10);
    rt_kprintf("thread2 got static mutex\n");
    rt_thread_delay(RT_TICK_PER_SECOND);
    rt_kprintf("thread2 release static mutex\n");
    rt_mutex_release(&static_mutex);

    /* 2. dynamic mutex test */
    rt_kprintf("thread2 try to get dynamic mutex\n");
    rt_mutex_take(dynamic_mutex, 10);
    rt_kprintf("thread2 got dynamic mutex\n");
    rt_thread_delay(RT_TICK_PER_SECOND);
    rt_kprintf("thread2 release dynamic mutex\n");
    rt_mutex_release(dynamic_mutex);
}

int demo_init(void)
{
    rt_err_t result;

    /* ��ʼ����̬������ */
    result = rt_mutex_init(&static_mutex, "smutex", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("init static mutex failed.\n");
        return -1;
    }

    /* ����һ����̬������ */
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_FIFO);
    if (dynamic_mutex == RT_NULL)
    {
        rt_kprintf("create dynamic mutex failed.\n");
        return -1;
    }


    rt_thread_init(&thread1,
                   "thread1",
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),11,5);
    rt_thread_startup(&thread1);


    rt_thread_init(&thread2,
                   "thread2",
                   rt_thread_entry2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),10,5);
    rt_thread_startup(&thread2);

    return 0;
}
