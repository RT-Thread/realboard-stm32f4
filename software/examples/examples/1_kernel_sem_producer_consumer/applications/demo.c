/*
 * �����嵥������������������
 *
 * ��������н����������߳�����ʵ������������������
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* �����̶߳�ջ�Ĵ�С */
#define THREAD_STACK_SIZE 1024
/* �������5��Ԫ���ܹ������� */
#define MAXSEM 5

/* ���ڷ����������������� */
rt_uint32_t array[MAXSEM];
/* ָ�������ߡ���������array�����еĶ�дλ�� */
static rt_uint32_t set, get;

struct rt_semaphore sem_lock;
struct rt_semaphore sem_empty, sem_full;

/* �������߳���� */
void producer_thread_entry(void* parameter)
{
    int cnt = 0;

    /* ����20�� */
    while (cnt < 20)
    {
        /* ��ȡһ����λ */
        rt_sem_take(&sem_empty, RT_WAITING_FOREVER);

        /* �޸�array���ݣ����� */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        array[set%MAXSEM] = cnt + 1;
        rt_kprintf("the producer generates a number: %d\n", array[set%MAXSEM]);
        set++;
        rt_sem_release(&sem_lock);

        /* ����һ����λ */
        rt_sem_release(&sem_full);
        cnt++;

        /* ��ͣһ��ʱ�� */
        //rt_thread_delay(15);
    }

    rt_kprintf("the producer exit!\n");
}

/* �������߳���� */
void consumer_thread_entry(void* parameter)
{
    rt_uint32_t no;
    rt_uint32_t sum;

    /* ��n���̣߳�����ڲ��������� */
    no = (rt_uint32_t)parameter;
    sum = 0;

    while(1)
    {
        /* ��ȡһ����λ */
        rt_sem_take(&sem_full, RT_WAITING_FOREVER);

        /* �ٽ������������в��� */
        rt_sem_take(&sem_lock, RT_WAITING_FOREVER);
        sum += array[get%MAXSEM];
        rt_kprintf("the consumer[%d] get a number: %d\n", no, array[get%MAXSEM] );
        get++;
        rt_sem_release(&sem_lock);

        /* �ͷ�һ����λ */
        rt_sem_release(&sem_empty);

        /* ������������20����Ŀ��ֹͣ���������߳���Ӧֹͣ */
        if (get == 20) 
            break;

        /* ��ͣһС��ʱ�� */
        rt_thread_delay(10);
    }

    rt_kprintf("the consumer[%d] exits, sum is %d \n ", no, sum);
}

int demo_init(void)
{
    rt_thread_t tid;
    rt_err_t result;

    /* ��ʼ��3���ź��� */
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
