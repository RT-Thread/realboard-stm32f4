/*
 *  shell implementation for finsh shell.
 *
 * COPYRIGHT (C) 2006 - 2013, RT-Thread Development Team
 *
 *  This file is part of RT-Thread (http://www.rt-thread.org)
 *  Maintainer: bernard.xiong <bernard.xiong at gmail.com>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-06-02     Bernard      Add finsh_get_prompt function declaration
 */

#ifndef __SHELL_H__
#define __SHELL_H__

#include <rtthread.h>
#include "finsh.h"

#define FINSH_USING_HISTORY
#ifndef FINSH_THREAD_PRIORITY
#define FINSH_THREAD_PRIORITY 20
#endif
#ifndef FINSH_THREAD_STACK_SIZE
#define FINSH_THREAD_STACK_SIZE 2048
#endif
#define FINSH_CMD_SIZE		80

#define FINSH_OPTION_ECHO	0x01
#if defined(RT_USING_DFS) && defined(DFS_USING_WORKDIR)
#define FINSH_PROMPT		finsh_get_prompt()
const char* finsh_get_prompt(void);
#else
#define FINSH_PROMPT		"finsh>>"
#endif

#ifdef FINSH_USING_HISTORY
	#ifndef FINSH_HISTORY_LINES
		#define FINSH_HISTORY_LINES 5
	#endif
#endif

enum input_stat
{
	WAIT_NORMAL,
	WAIT_SPEC_KEY,
	WAIT_FUNC_KEY,
};
struct finsh_shell
{
	struct rt_semaphore rx_sem;

	enum input_stat stat;

	rt_uint8_t echo_mode:1;

#ifdef FINSH_USING_HISTORY
	rt_uint16_t current_history;
	rt_uint16_t history_count;

	char cmd_history[FINSH_HISTORY_LINES][FINSH_CMD_SIZE];
#endif

#ifndef FINSH_USING_MSH_ONLY
	struct finsh_parser parser;
#endif

	char line[FINSH_CMD_SIZE];
	rt_uint8_t line_position;
	rt_uint8_t line_curpos;

	rt_device_t device;
};

void finsh_set_echo(rt_uint32_t echo);
rt_uint32_t finsh_get_echo(void);

int finsh_system_init(void);
void finsh_set_device(const char* device_name);
const char* finsh_get_device(void);

#endif
