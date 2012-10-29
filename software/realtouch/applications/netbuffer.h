#ifndef __NET_BUF_H__
#define __NET_BUF_H__

#include <rtthread.h>
#include "board.h"

/* SRAM buffer pool routine */
rt_size_t sbuf_get_size(void);
void* sbuf_alloc(void);
void sbuf_release(void* ptr);

#endif
