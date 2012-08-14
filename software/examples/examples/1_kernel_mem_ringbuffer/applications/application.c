#include <rtthread.h>
#include "rtdevice.h"


static rt_sem_t sem = RT_NULL;

static rt_uint8_t working_buffer[256];
struct rt_ringbuffer rb;

/* 指向线程控制块的指针 */
static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

/* 线程1入口 */
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

/* 线程2入口，线程2的优先级比线程1低，应该线程1先获得执行。*/
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

        rt_sem_take(sem,RT_WAITING_FOREVER);

        result = rt_ringbuffer_put(&rb,&data_buffer[0],33);
        rt_kprintf("write buffer success!\n");
        
        rt_sem_release(sem);
        
        rt_thread_delay(10);
    }
}

int rt_application_init()
{
    
    rt_ringbuffer_init(&rb,working_buffer,256);

    sem = rt_sem_create("sem",1,RT_IPC_FLAG_FIFO);
    if (sem == RT_NULL)
    {
        return 0;
    }
    
    /* 创建线程1 */
    tid1 = rt_thread_create("t1",
        thread1_entry, RT_NULL, /* 线程入口是thread1_entry, 入口参数是RT_NULL */
        512, 8, 10);
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    /* 创建线程2 */
    tid2 = rt_thread_create("t2",
        thread2_entry, RT_NULL, /* 线程入口是thread2_entry, 入口参数是RT_NULL */
        512, 7, 10);
    if (tid2 != RT_NULL)
        rt_thread_startup(tid2);

    return 0;
}
