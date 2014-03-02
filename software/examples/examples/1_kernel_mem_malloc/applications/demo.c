 /*
	这份demo用于演示动态内存分配之rt_malloc、rt_free
 */
#include <rtthread.h>
#ifdef RT_USING_FINSH
#include <finsh.h>
#include <shell.h>
#endif


/* 线程TCB和栈*/
ALIGN(RT_ALIGN_SIZE)
char thread1_stack[512];
struct rt_thread thread1;

/* 线程入口*/
void thread1_entry(void* parameter)
{
    int i,j;
    char *ptr[20]; /* 用于放置20个分配内存块的指针*/
    /* 对指针清零*/
    for (i = 0; i < 20; i ++) 
        ptr[i] = RT_NULL;
    for(j = 0; j < 2; j ++ )
    {
        for (i = 0; i <20; i++)
        {
        /* 每次分配(1 <<i)大小字节数的内存空间*/
            ptr[i] = rt_malloc(1 <<i);
            /* 如果分配成功*/
            if (ptr[i] != RT_NULL)
            {
                rt_kprintf("get memory: 0x%x\n", ptr[i]);
                /* 释放内存块*/
                rt_free(ptr[i]);
                ptr[i] = RT_NULL;
            }
        }
    }
}


int demo_init(void)
{
    /* 初始化线程对象*/
    rt_thread_init(&thread1,
    "thread1",
    thread1_entry, RT_NULL,
    &thread1_stack[0], sizeof(thread1_stack),
    10, 100);
    rt_thread_startup(&thread1);

    return 0;
}
