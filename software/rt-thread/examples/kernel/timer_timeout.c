/*
 * �����嵥����Ϣ��������
 *
 * �������ᴴ��3����̬�̣߳�һ���̻߳����Ϣ��������ȡ��Ϣ��һ���̻߳ᶨʱ����
 * Ϣ���з�����Ϣ��һ���̻߳ᶨʱ����Ϣ���з��ͽ�����Ϣ��
 */
#include <rtthread.h>
#include "tc_comm.h"

/* ָ���߳̿��ƿ��ָ�� */
static rt_thread_t tid = RT_NULL;

/* ��Ϣ���п��ƿ� */
static struct rt_messagequeue mq;
/* ��Ϣ�������õ��ķ�����Ϣ���ڴ�� */
static char msg_pool[2048];

/* ��ʱ���Ŀ��ƿ� */
static struct rt_timer timer;
static rt_uint16_t no = 0;
static void timer_timeout(void* parameter)
{
    char buf[32];
    rt_uint32_t length;

    length = rt_snprintf(buf, sizeof(buf), "message %d", no++);
    rt_mq_send(&mq, &buf[0], length);
}

/* �߳���ں��� */
static void thread_entry(void* parameter)
{
    char buf[64];
    rt_err_t result;

    /* ��ʼ����ʱ�� */
    rt_timer_init(&timer, "timer",  /* ��ʱ�������� timer1 */
        timer_timeout, /* ��ʱʱ�ص��Ĵ����� */
        RT_NULL, /* ��ʱ��������ڲ��� */
        1, /* ��ʱ���ȣ���OS TickΪ��λ����1��OS Tick */
        RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */

    while (1)
    {
        rt_memset(&buf[0], 0, sizeof(buf));

        /* ����Ϣ�����н�����Ϣ */
        result = rt_mq_recv(&mq, &buf[0], sizeof(buf), 1);
        if (result == RT_EOK)
        {
            rt_kprintf("recv msg: %s\n", buf);
        }
        else if (result == -RT_ETIMEOUT)
        {
            rt_kprintf("recv msg timeout\n");
        }
    }
}

int timer_timeout_init()
{
    /* ��ʼ����Ϣ���� */
    rt_mq_init(&mq, "mqt", 
        &msg_pool[0], /* �ڴ��ָ��msg_pool */ 
        128 - sizeof(void*), /* ÿ����Ϣ�Ĵ�С�� 128 - void* */
        sizeof(msg_pool), /* �ڴ�صĴ�С��msg_pool�Ĵ�С */
        RT_IPC_FLAG_FIFO); /* ����ж���̵߳ȴ������������ȵõ��ķ���������Ϣ */

    /* �����߳� */
    tid = rt_thread_create("t",
        thread_entry, RT_NULL, /* �߳������thread_entry, ��ڲ�����RT_NULL */
        THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    else
        tc_stat(TC_STAT_END | TC_STAT_FAILED);

    return 0;
}

#ifdef RT_USING_TC
static void _tc_cleanup()
{
    /* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
    rt_enter_critical();

    /* ɾ���߳� */
    if (tid != RT_NULL && tid->stat != RT_THREAD_CLOSE)
        rt_thread_delete(tid);

    /* ִ����Ϣ���ж������� */
    rt_mq_detach(&mq);
    /* ִ�ж�ʱ������ */
    rt_timer_detach(&timer);

    /* ���������� */
    rt_exit_critical();

    /* ����TestCase״̬ */
    tc_done(TC_STAT_PASSED);
}

int _tc_timer_timeout()
{
    /* ����TestCase����ص����� */
    tc_cleanup(_tc_cleanup);
    timer_timeout_init();

    /* ����TestCase���е��ʱ�� */
    return 100;
}
/* ����������finsh shell�� */
FINSH_FUNCTION_EXPORT(_tc_timer_timeout, a thread timer testcase);
#else
/* �û�Ӧ����� */
int rt_application_init()
{
    timer_timeout_init();

    return 0;
}
#endif
