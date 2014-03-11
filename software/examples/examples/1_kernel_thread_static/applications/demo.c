/*
	��demo������ʾ��̬�߳�
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* �߳�1���ƿ� */
static struct rt_thread thread1;
/* �߳�1ջ */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread1_stack[512];

/* �߳�2���ƿ� */
static struct rt_thread thread2;
/* �߳�2ջ */
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t thread2_stack[512];

/* �߳�1��� */
static void thread1_entry(void *parameter)
{
    rt_uint32_t count = 0;

    while (1)
    {
        /* �߳�1���õ����ȼ����У�һֱ��ӡ����ֵ */
        rt_kprintf("thread count: %d\n", count ++);
        /* �߳�1��ʱ1�� */
        rt_thread_delay(RT_TICK_PER_SECOND);
    }
}

/* �߳�2��� */
static void thread2_entry(void *parameter)
{
    /* �߳�2ӵ�нϸߵ����ȼ�������ռ�߳�1�����ִ�� */

    /* �߳�2��������˯��10�� */
    rt_thread_delay(RT_TICK_PER_SECOND * 10);

    /*
     * �߳�2���Ѻ�ֱ��ִ���߳�1���룬�߳�1���Ӿ����̶߳�����ɾ��
     */
    rt_thread_detach(&thread1);

    /*
     * �߳�2��������10��OS TickȻ���˳�
     */
    rt_thread_delay(10);
}


int demo_init(void)
{
    rt_err_t result;

    /* ��ʼ���߳�1 */
    result = rt_thread_init(&thread1, "t1", /* �߳�����t1 */
        thread1_entry, RT_NULL, /* �̵߳������thread1_entry����ڲ�����RT_NULL*/
        &thread1_stack[0], sizeof(thread1_stack), /* �߳�ջ��thread1_stack */
        7, 10);
    if (result == RT_EOK) /* ���������ȷ�������߳�1 */
        rt_thread_startup(&thread1);

    /* ��ʼ���߳�2 */
    result = rt_thread_init(&thread2, "t2", /* �߳�����t2 */
        thread2_entry, RT_NULL, /* �̵߳������thread2_entry����ڲ�����RT_NULL*/
        &thread2_stack[0], sizeof(thread2_stack), /* �߳�ջ��thread2_stack */
        6, 10);
    if (result == RT_EOK) /* ���������ȷ�������߳�2 */
        rt_thread_startup(&thread2);

    return 0;
}
