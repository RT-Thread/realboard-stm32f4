/*
 * �����嵥����Ϣ��������
 *
 * �������ᴴ��3����̬�̣߳�һ���̻߳����Ϣ��������ȡ��Ϣ��һ���̻߳ᶨʱ����
 * Ϣ���з�����Ϣ��һ���̻߳����Ϣ���з��ͽ�����Ϣ��
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


#define MSG_VIP "over"
/* ��Ϣ���п��ƿ� */
static struct rt_messagequeue mq;
/* ��Ϣ�������õ��ķ�����Ϣ���ڴ�� */
static char msg_pool[2048];

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;
/* �߳�1��ں��� */
static void thread1_entry(void* parameter)
{
    char buf[120];

    while (1)
    {
        rt_memset(&buf[0], 0, sizeof(buf));

        /* ����Ϣ�����н�����Ϣ */
        if (rt_mq_recv(&mq, &buf[0], sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: recv msg from msg queue, the content:%s\n", buf);
            
            /* ����Ƿ��յ��˽�����Ϣ */
            if (strcmp(buf, MSG_VIP) == 0)
                break;
        }

        /* ��ʱ1s */
        rt_thread_delay(RT_TICK_PER_SECOND);
    }

    rt_kprintf("thread1: got an urgent message, leave\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;
/* �߳�2��� */
static void thread2_entry(void* parameter)
{
    int i, result;
    char buf[120];

    i = 0;
    while (1)    
    {
        rt_snprintf(buf, sizeof(buf), "this is message No.%d", i);

        /* ������Ϣ����Ϣ������ */
        result = rt_mq_send(&mq, &buf[0], sizeof(buf));
        if ( result == -RT_EFULL)
            break;

        rt_kprintf("thread2: send message - %s\n", buf);

        i++;
    }

    rt_kprintf("message queue full, thread2 leave\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread3_stack[1024];
struct rt_thread thread3;
/* �߳�3��ں��� */
static void thread3_entry(void* parameter)
{
    char msg[] = MSG_VIP;
    int result;

    rt_thread_delay(RT_TICK_PER_SECOND * 5);
    rt_kprintf("thread3: send an urgent message <%s> \n", msg);

    /* ���ͽ�����Ϣ����Ϣ������ */
    do {
        result = rt_mq_urgent(&mq, &msg[0], sizeof(msg));
        
        if (result != RT_EOK)
            rt_thread_delay(20);
    } while (result != RT_EOK);

}

int demo_init(void)
{
    rt_thread_t init_thread;
    rt_err_t result;

    /* ��ʼ����Ϣ���� */
    result = rt_mq_init(&mq, "mqt", 
        &msg_pool[0], /* �ڴ��ָ��msg_pool */ 
        128 - sizeof(void*), /* ÿ����Ϣ�Ĵ�С�� 128 - void* */
        sizeof(msg_pool), /* �ڴ�صĴ�С��msg_pool�Ĵ�С */
        RT_IPC_FLAG_FIFO);/* ����ж���̵߳ȴ������������ȵõ��ķ���������Ϣ */
    if (result != RT_EOK)
    {
        rt_kprintf("init message queue failed.\n");
        return -1;
    }


    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),10,50);
    rt_thread_startup(&thread1);


    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),10,50);
    rt_thread_startup(&thread2);


    rt_thread_init(&thread3,
                   "thread3",
                   thread3_entry,
                   RT_NULL,
                   &thread3_stack[0],
                   sizeof(thread3_stack),10,50);
    rt_thread_startup(&thread3);

    return 0;
}
