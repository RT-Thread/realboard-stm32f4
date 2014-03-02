 /*
	���demo������ʾ���λ�����
 */
#include <rtthread.h>
#include "rtdevice.h" //for rt_ringbuffer
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif



static rt_sem_t sem = RT_NULL;

static rt_uint8_t working_buffer[256];
struct rt_ringbuffer rb;

/* ָ���߳̿��ƿ��ָ�� */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

/* �߳�1��� */
static void thread1_entry(void* parameter)
{
    rt_bool_t result;
    rt_uint8_t data_buffer[33];
    rt_uint32_t i = 1;

    while(i--)
    {
        rt_sem_take (sem,RT_WAITING_FOREVER);
        result = rt_ringbuffer_get(&rb,&data_buffer[0],33);
        rt_sem_release(sem);

        rt_kprintf("%s\n",data_buffer);
        
        rt_thread_delay(5);
    }

}

/* �߳�2��ڣ��߳�2�����ȼ����߳�1�ͣ�Ӧ���߳�1�Ȼ��ִ�С�*/
static void thread2_entry(void *parameter)
{
    rt_bool_t result;
    rt_uint32_t index,setchar,i = 1;
    rt_uint8_t data_buffer[33];

    setchar = 0x21;
    while(i--)
    {
        for (index = 0; index < 32; index++)
        {
            data_buffer[index] = setchar;
            if (++setchar == 0x7f)
            {
                setchar = 0x21;
            }
        }
        data_buffer[32] = '\0'; 

        result = rt_ringbuffer_put(&rb,&data_buffer[0],33);
        rt_kprintf("write buffer success!\n");
        
        rt_sem_release(sem);
        
        rt_thread_delay(10);
    }
}

int demo_init(void)
{
    rt_ringbuffer_init(&rb,working_buffer,256);

    sem = rt_sem_create("sem",0,RT_IPC_FLAG_FIFO);
    if (sem == RT_NULL)
    {
        return 0;
    }
    
    /* �����߳�1 */
    tid1 = rt_thread_create("t1",
        thread1_entry, RT_NULL, /* �߳������thread1_entry, ��ڲ�����RT_NULL */
        512, 8, 10);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* �����߳�2 */
    tid2 = rt_thread_create("t2",
        thread2_entry, RT_NULL, /* �߳������thread2_entry, ��ڲ�����RT_NULL */
        512, 7, 10);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    return 0;
}
