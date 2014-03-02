 /*
	���demo������ʾ��̬�ڴ����֮rt_malloc��rt_free
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
    int i,j;
    char *ptr[20]; /* ���ڷ���20�������ڴ���ָ��*/
    /* ��ָ������*/
    for (i = 0; i < 20; i ++) 
        ptr[i] = RT_NULL;
    for(j = 0; j < 2; j ++ )
    {
        for (i = 0; i <20; i++)
        {
        /* ÿ�η���(1 <<i)��С�ֽ������ڴ�ռ�*/
            ptr[i] = rt_malloc(1 <<i);
            /* �������ɹ�*/
            if (ptr[i] != RT_NULL)
            {
                rt_kprintf("get memory: 0x%x\n", ptr[i]);
                /* �ͷ��ڴ��*/
                rt_free(ptr[i]);
                ptr[i] = RT_NULL;
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
