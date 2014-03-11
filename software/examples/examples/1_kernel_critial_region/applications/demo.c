/*
  这份demo用于演示系统临界区问题
*/

#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif



static int share_var;

ALIGN(RT_ALIGN_SIZE)

static char thread1_stack[1024];
struct rt_thread thread1;

static void rt_thread_entry1(void* parameter)
{
    int i;
    share_var = 0;
    rt_kprintf("share_var = %d\n", share_var);

    for(i=0; i<100000; i++)
    {
        share_var ++;
    }
    rt_kprintf("\t share_var = %d\n", share_var);
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;

static void rt_thread_entry2(void* parameter)
{
    rt_thread_delay(1); /* rt_thread_delay(1000);*/
    share_var ++;
}

int demo_init(void)
{
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
