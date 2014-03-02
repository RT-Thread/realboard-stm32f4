/*
 ���demo������ʾ�¼�����
	
 ����ᴴ��3����̬�̼߳���ʼ��һ����̬�¼�����
 һ���̵߳ȴ����¼��������Խ����¼���
 һ���̶߳�ʱ�����¼� (�¼�3)
 һ���̶߳�ʱ�����¼� (�¼�5)
*/
 

#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* �¼����ƿ� */
static struct rt_event event;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;
/* �߳�1��ں��� */
static void thread1_entry(void *param)
{
    rt_uint32_t e;

    /* receive first event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &e) == RT_EOK)
    {
        rt_kprintf("thread1: AND recv event 0x%x\n", e);
    }

    rt_kprintf("thread1: delay 1s to prepare second event\n");
    rt_thread_delay(RT_TICK_PER_SECOND);

    /* receive second event */
    if (rt_event_recv(&event, ((1 << 3) | (1 << 5)),
        RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
        RT_WAITING_FOREVER, &e) == RT_EOK)
    {
        rt_kprintf("thread1: OR recv event 0x%x\n", e);
    }
    rt_kprintf("thread1 leave.\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;
/* �߳�2��� */
static void thread2_entry(void *param)
{
    rt_kprintf("thread2: send event1\n");
    rt_event_send(&event, (1 << 3));
    rt_kprintf("thread2 leave.\n");
}

ALIGN(RT_ALIGN_SIZE)
static char thread3_stack[1024];
struct rt_thread thread3;
/* �߳�3��ں��� */
static void thread3_entry(void *param)
{
    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));

    rt_thread_delay(20);

    rt_kprintf("thread3: send event2\n");
    rt_event_send(&event, (1 << 5));

    rt_kprintf("thread3 leave.\n");
}

int demo_init(void)
{
    rt_thread_t init_thread;
    rt_err_t result;

    /* ��ʼ���¼����� */
    rt_event_init(&event, "event", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("init event failed.\n");
        return -1;
    }

    rt_thread_init(&thread1,
                   "thread1",
                   thread1_entry,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),8,50);
    rt_thread_startup(&thread1);


    rt_thread_init(&thread2,
                   "thread2",
                   thread2_entry,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),9,50);
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
