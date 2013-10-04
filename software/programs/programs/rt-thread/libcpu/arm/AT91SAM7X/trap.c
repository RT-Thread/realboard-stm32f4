/*
 * File      : trap.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-08-25     Bernard      first version
 */

#include <rtthread.h>
#include <rthw.h>

#include "AT91SAM7X256.h"

/**
 * @addtogroup AT91SAM7
 */
/*@{*/

void rt_hw_trap_irq()
{
	rt_isr_handler_t hander = (rt_isr_handler_t)AT91C_BASE_AIC->AIC_IVR;

	hander(AT91C_BASE_AIC->AIC_ISR);

	/* end of interrupt */
	AT91C_BASE_AIC->AIC_EOICR = 0;
}

void rt_hw_trap_fiq()
{
    rt_kprintf("fast interrupt request\n");
}

extern struct rt_thread* rt_current_thread;
void rt_hw_trap_abort()
{
	rt_kprintf("Abort occured!!! Thread [%s] suspended.\n",rt_current_thread->name);
	rt_thread_suspend(rt_current_thread);
	rt_schedule();

}
/*@}*/
