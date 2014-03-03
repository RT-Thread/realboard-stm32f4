/*
 * File      : k9f2g08u0b.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author           Notes
 * 2012-11-24     heyuanjie87      first implementation
 */

#ifndef __K9F2G08U0B_H__
#define __K9F2G08U0B_H__

#include <rtdevice.h>

/* nandflash confg */
#define PAGES_PER_BLOCK         64
#define PAGE_DATA_SIZE          2048
#define PAGE_OOB_SIZE           64
#define NAND_MARK_SPARE_OFFSET  4

#define CMD_AREA                   (uint32_t)(0x010000)  /* A16 = CLE  high */
#define ADDR_AREA                  (uint32_t)(0x020000)  /* A17 = ALE high */
#define DATA_AREA                  (uint32_t)(0x000000)

/* FSMC NAND memory command */
#define	NAND_CMD_READ_1             ((uint8_t)0x00)
#define	NAND_CMD_READ_TRUE          ((uint8_t)0x30)

#define	NAND_CMD_RDCOPYBACK         ((uint8_t)0x00)
#define	NAND_CMD_RDCOPYBACK_TRUE    ((uint8_t)0x35)

#define NAND_CMD_PAGEPROGRAM        ((uint8_t)0x80)
#define NAND_CMD_PAGEPROGRAM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_COPYBACKPGM        ((uint8_t)0x85)
#define NAND_CMD_COPYBACKPGM_TRUE   ((uint8_t)0x10)

#define NAND_CMD_ERASE0             ((uint8_t)0x60)
#define NAND_CMD_ERASE1             ((uint8_t)0xD0)

#define NAND_CMD_READID             ((uint8_t)0x90)
#define NAND_CMD_STATUS             ((uint8_t)0x70)
#define NAND_CMD_RESET              ((uint8_t)0xFF)

#define NAND_CMD_CACHEPGM           ((uint8_t)0x80)
#define NAND_CMD_CACHEPGM_TRUE      ((uint8_t)0x15)

#define NAND_CMD_RANDOMIN           ((uint8_t)0x85)
#define NAND_CMD_RANDOMOUT          ((uint8_t)0x05)
#define NAND_CMD_RANDOMOUT_TRUE     ((uint8_t)0xE0)

#define NAND_CMD_CACHERD_START      ((uint8_t)0x00)
#define NAND_CMD_CACHERD_START2     ((uint8_t)0x31)
#define NAND_CMD_CACHERD_EXIT       ((uint8_t)0x34)

#define NAND_PAGE_SIZE    (2048)



struct stm32f4_nand
{
    rt_uint8_t id[5];
    struct rt_mutex lock;
    struct rt_completion comp;
};

void rt_hw_mtd_nand_init(void);

#endif /* __K9F2G08U0B_H__ */
