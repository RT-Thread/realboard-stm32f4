 /*
	���demo������ʾ��̬�ڴ����֮rt_realloc��rt_free
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* �߳�TCB��ջ*/
ALIGN(RT_ALIGN_SIZE)
char thread1_stack[512];
struct rt_thread thread1;

/* �߳����*/
void thread1_entry(void* parameter)
{
    int i,j = 1;
    char *ptr[10]; /* ���ڷ���10�������ڴ���ָ��*/
    /* ��ָ������*/
    for (i = 0; i <10; i ++) 
        ptr[i] = RT_NULL;
    while(j--)
    {
        for (i = 0; i <10; i++)
        {
        /* ÿ�η���16�ֽڵ��ڴ�ռ�*/
            ptr[i] = rt_malloc(10);
            /* �������ɹ�*/
            if (ptr[i] != RT_NULL)
            {
                rt_kprintf("get memory: 0x%x\n", ptr[i]);
                 rt_realloc(ptr[i],16);
                /* �������ɹ�*/
                if (ptr[i] != RT_NULL)
                {
                    rt_kprintf("memory realloc success!\n");
                    /* �ͷ��ڴ��*/
                    rt_free(ptr[i]);
                    rt_kprintf("memory free success!\n");
                    ptr[i] = RT_NULL;
                }
            }
        }
    }
}

int demo_init(void)
{
    /* ��ʼ���̶߳���*/
    rt_thread_init(&thread1,
    "thread1",
    thread1_entry, RT_NULL,
    &thread1_stack[0], sizeof(thread1_stack),
    10, 100);
    rt_thread_startup(&thread1);

    return 0;
}
