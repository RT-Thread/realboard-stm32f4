#include <rtthread.h>

void init_entry(void *param)
{
	application_init();
}

int rt_application_init()
{
    rt_thread_t tid;

    tid = rt_thread_create("init",
                           init_entry, RT_NULL,
                           2048, RT_THREAD_PRIORITY_MAX / 4, 20);

    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
