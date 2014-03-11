/*
	��demo������ʾ�߳��ó�
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* �߳�1��� */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    for (; count < 5; count ++)
    {
        /* ��ӡ�߳�1����� */
        rt_kprintf("thread1: count = %d\n", count);
        /* ִ��yield��Ӧ���л���thread2ִ�� */
        rt_thread_yield();
    }
}

/* �߳�2��� */
static void thread2_entry(void *parameter)
{
    rt_uint32_t count = 0;

    for (; count < 5; count ++)
    {
        /* ��ӡ�߳�2����� */
        rt_thread_yield();
        rt_kprintf("thread2: count = %d\n", count);
    }
}


int demo_init(void)
{
	/* ָ���߳̿��ƿ��ָ�� */
 	rt_thread_t tid1 = RT_NULL;
 	rt_thread_t tid2 = RT_NULL;

    tid1 = rt_thread_create("thread",
        thread1_entry, RT_NULL, /* �߳������thread1_entry, ��ڲ�����RT_NULL */
        512, 6, 100);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* �����߳�2 */
    tid2 = rt_thread_create("thread",
        thread2_entry, RT_NULL, /* �߳������thread2_entry, ��ڲ�����RT_NULL */
        512, 6, 100);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    return 0;
}
