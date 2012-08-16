/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>

#include "board.h"

/**
 * @addtogroup STM32
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
    /* Set the Vector Table base location at 0x20000000 */
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/**
 * This fucntion returns milliseconds since system passed
 */
rt_uint32_t rt_hw_tick_get_millisecond(void)
{
    rt_tick_t tick;
    rt_uint32_t value;

#define TICK_MS (1000/RT_TICK_PER_SECOND)

    tick = rt_tick_get();
    value = tick * TICK_MS + (SysTick->LOAD - SysTick->VAL) * TICK_MS / SysTick->LOAD;

    return value;
}

/**
 * This fucntion returns microseconds since system passed
 */
rt_uint32_t rt_hw_tick_get_microsecond(void)
{
    rt_tick_t tick;
    rt_uint32_t value;

#define TICK_US	(1000000/RT_TICK_PER_SECOND)

    tick = rt_tick_get();
    value = tick * TICK_US + (SysTick->LOAD - SysTick->VAL) * TICK_US / SysTick->LOAD;

    return value;
}

/*
+-------------------+--------------------+------------------+-------------------+
+                       STM32 FSMC pins assignment                              +
+-------------------+--------------------+------------------+-------------------+
| PD0  <-> FSMC_D2  | PE0  <-> FSMC_NBL0 | PF0  <-> FSMC_A0 | PG0  <-> FSMC_A10 |
| PD1  <-> FSMC_D3  | PE1  <-> FSMC_NBL1 | PF1  <-> FSMC_A1 | PG1  <-> FSMC_A11 |
| PD4  <-> FSMC_NOE | PE3  <-> FSMC_A19  | PF2  <-> FSMC_A2 | PG2  <-> FSMC_A12 |
| PD5  <-> FSMC_NWE | PE4  <-> FSMC_A20  | PF3  <-> FSMC_A3 | PG3  <-> FSMC_A13 |
| PD8  <-> FSMC_D13 | PE7  <-> FSMC_D4   | PF4  <-> FSMC_A4 | PG4  <-> FSMC_A14 |
| PD9  <-> FSMC_D14 | PE8  <-> FSMC_D5   | PF5  <-> FSMC_A5 | PG5  <-> FSMC_A15 |
| PD10 <-> FSMC_D15 | PE9  <-> FSMC_D6   | PF12 <-> FSMC_A6 | PG9  <-> FSMC_NE2 |
| PD11 <-> FSMC_A16 | PE10 <-> FSMC_D7   | PF13 <-> FSMC_A7 | PG12 <-> FSMC_NE4 |
| PD12 <-> FSMC_A17 | PE11 <-> FSMC_D8   | PF14 <-> FSMC_A8 |-------------------+
| PD13 <-> FSMC_A18 | PE12 <-> FSMC_D9   | PF15 <-> FSMC_A9 |
| PD14 <-> FSMC_D0  | PE13 <-> FSMC_D10  |------------------+
| PD15 <-> FSMC_D1  | PE14 <-> FSMC_D11  |
| PD7  <-> FSMC_NE1 | PE15 <-> FSMC_D12  |
+-------------------+--------------------+
*/
void fsmc_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIOs clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                           RCC_AHB1Periph_GPIOG, ENABLE);

    /* Enable FSMC clock */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    /* GPIOD configuration */
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);

    /* GPIOE configuration */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource1 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource3 , GPIO_AF_FSMC);
    //GPIO_PinAFConfig(GPIOE, GPIO_PinSource4 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);

    /* GPIOF configuration */
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource0 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource2 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource3 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource4 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource5 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource12 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource13 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource14 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource15 , GPIO_AF_FSMC);

    /* GPIOG configuration */
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource0 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource1 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource2 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource3 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource4 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource5 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource9 , GPIO_AF_FSMC);
    GPIO_PinAFConfig(GPIOG, GPIO_PinSource12 , GPIO_AF_FSMC);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
                                  | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
                                  | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
                                  | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13
                                  | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_3 |
                                  GPIO_Pin_7  | GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 |
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 |
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                  GPIO_Pin_4  | GPIO_Pin_5  |
                                  GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                  GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_9 | GPIO_Pin_12;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void mco_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* MCO configure */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
    RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* NVIC Configuration */
    NVIC_Configuration();

    /* Configure the SysTick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    rt_hw_usart_init();
#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    fsmc_gpio_init();

    mco_config();
}

/*@}*/
