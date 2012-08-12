/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <stdio.h>

#include "stm32f4xx.h"
#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#include "stm32_eth.h"
#endif

void rt_init_thread_entry(void* parameter)
{

}

/* 信号量控制块 */
static struct rt_semaphore static_sem;
/* 指向信号量的指针 */
static rt_sem_t dynamic_sem = RT_NULL;

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;
static void rt_thread_entry1(void* parameter)
{
    rt_err_t result;
    rt_tick_t tick;

/* 1. staic semaphore demo */
    /* 获得当前的OS Tick */
    tick = rt_tick_get();

    /* 试图持有信号量，最大等待10个OS Tick后返回 */
    result = rt_sem_take(&static_sem, 10);
    if (result == -RT_ETIMEOUT)
    {
        /* 超时后判断是否刚好是10个OS Tick */
        if (rt_tick_get() - tick != 10)
        {
            rt_sem_detach(&static_sem);
            return;
        }
        rt_kprintf("take semaphore timeout\n");
    }
    else
    {
        /* 因为没有其他地方释放信号量，所以不应该成功持有信号量，否则测试失败 */
        rt_kprintf("take a static semaphore, failed.\n");
        rt_sem_detach(&static_sem);
        return;
    }

    /* 释放一次信号量 */
    rt_sem_release(&static_sem);

    /* 永久等待方式持有信号量 */
    result = rt_sem_take(&static_sem, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        /* 不成功则测试失败 */
        rt_kprintf("take a static semaphore, failed.\n");
        rt_sem_detach(&static_sem);
        return;
    }

    rt_kprintf("take a staic semaphore, done.\n");

    /* 脱离信号量对象 */
    rt_sem_detach(&static_sem);

/* 2. dynamic semaphore test */

    tick = rt_tick_get();

    /* 试图持有信号量，最大等待10个OS Tick后返回 */
    result = rt_sem_take(dynamic_sem, 10);
    if (result == -RT_ETIMEOUT)
    {
        /* 超时后判断是否刚好是10个OS Tick */
        if (rt_tick_get() - tick != 10)
        {
            rt_sem_delete(dynamic_sem);
            return;
        }
        rt_kprintf("take semaphore timeout\n");
    }
    else
    {
        /* 因为没有其他地方释放信号量，所以不应该成功持有信号量，否则测试失败 */
        rt_kprintf("take a dynamic semaphore, failed.\n");
        rt_sem_delete(dynamic_sem);
        return;
    }

    /* 释放一次信号量 */
    rt_sem_release(dynamic_sem);

    /* 永久等待方式持有信号量 */
    result = rt_sem_take(dynamic_sem, RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        /* 不成功则测试失败 */
        rt_kprintf("take a dynamic semaphore, failed.\n");
        rt_sem_delete(dynamic_sem);
        return;
    }

    rt_kprintf("take a dynamic semaphore, done.\n");
    /* 删除信号量对象 */
    rt_sem_delete(dynamic_sem);
}

int rt_application_init()
{
    rt_thread_t init_thread;
    rt_err_t result;

    /* 初始化静态信号量，初始值是0 */
    result = rt_sem_init(&static_sem, "ssem", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("init dynamic semaphore failed.\n");
        return -1;
    }

    /* 创建一个动态信号量，初始值是0 */
    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO);
    if (dynamic_sem == RT_NULL)
    {
        rt_kprintf("create dynamic semaphore failed.\n");
        return -1;
    }

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    rt_thread_init(&thread1,
                   "thread1",
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),11,5);
    rt_thread_startup(&thread1);

    return 0;
}

/*@}*/
