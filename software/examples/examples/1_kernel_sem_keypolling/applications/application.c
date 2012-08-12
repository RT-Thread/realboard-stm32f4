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

#define KEY_PORT  GPIOF
#define KEY_PIN   (GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |\
                     GPIO_Pin_10| GPIO_Pin_11)
#define KEY_CLCOK RCC_AHB1Periph_GPIOF

void rt_init_thread_entry(void* parameter)
{
//gpio init
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIOA GPIOB Periph clock enable */
    RCC_AHB1PeriphClockCmd(KEY_CLCOK , ENABLE);

    /* Configure Pin in input pushpull mode */
    GPIO_InitStructure.GPIO_Pin = KEY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

ALIGN(RT_ALIGN_SIZE)
static char thread1_stack[1024];
struct rt_thread thread1;

static int key;
static struct rt_semaphore sem;

static void rt_thread_entry1(void* parameter)
{
    int temp;
    
    while (1)
    {
        key = GPIO_ReadInputData(KEY_PORT);

        if (key & KEY_PIN)
        {
            temp = key;
            rt_thread_delay(RT_TICK_PER_SECOND / 50);
            key = GPIO_ReadInputData(KEY_PORT);
            if (key == temp)
                rt_sem_release(&sem);
        }
        rt_thread_delay(RT_TICK_PER_SECOND/10);
   }
}

ALIGN(RT_ALIGN_SIZE)
static char thread2_stack[1024];
struct rt_thread thread2;

static int key;
static void rt_thread_entry2(void* parameter)
{
    while (1)
    {
        rt_sem_take(&sem, RT_WAITING_FOREVER);

        if (key & KEY_PIN)
        {
            rt_kprintf("some keys has been pressed : %x\n", key);
        }
    }
}

int rt_application_init()
{
    rt_thread_t init_thread;
    rt_err_t result;
    
    result = rt_sem_init(&sem, "sem", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("error, init sem failed!\n");
        return 0;
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

    //------- init thread1
    rt_thread_init(&thread1,
                   "keyp", //producer
                   rt_thread_entry1,
                   RT_NULL,
                   &thread1_stack[0],
                   sizeof(thread1_stack),11,25);
    rt_thread_startup(&thread1);
    
    //------- init thread2
    rt_thread_init(&thread2,
                   "keyc", //consumer
                   rt_thread_entry2,
                   RT_NULL,
                   &thread2_stack[0],
                   sizeof(thread2_stack),11,24);
    rt_thread_startup(&thread2);
    return 0;
}

/*@}*/
