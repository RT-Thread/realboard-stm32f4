/*
 * File      : K9F2G08U0X.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2012-01-05     Bernard      first implementation
 * 2012-01-05     aozima       first implementation
 */
#include <rtdevice.h>
#include "K9F2G08U0X.h"

static struct rt_mtd_nand_device _partition[1];

/* RT-Thread MTD NAND driver interface. */

/**
 * This function read the id of nand flash
 *
 * @param device Pointer to MTD_NAND device
 *
 * @return RT_EOK - The nand flash is K9F2G08U0B
 *         RT_ERROR - Can not support this flash
 */
static rt_err_t nandflash_readid(struct rt_mtd_nand_device *device)
{
    return RT_EOK;
}

/**
 * This function read a page from nand flash
 *
 * @param device Pointer to MTD_NAND device
 * @param page Relative page offset of a logical partition
 * @param data Buffer of main data
 * @param data_len The length you want to read
 * @param spare Buffer of spare data
 * @param spare_len The length you want to read
 *
 * @return RT_MTD_EOK - No error occurs
 *         -RT_MTD_EECC - The ECC error can not be fixed
 */
static rt_err_t nandflash_readpage(struct rt_mtd_nand_device *device,
                                   rt_off_t                   page,
                                   rt_uint8_t                *data,
                                   rt_uint32_t                data_len,
                                   rt_uint8_t                *spare,
                                   rt_uint32_t                spare_len)
{
    return RT_EOK;
}

/**
 * This function write a page to nand flash
 *
 * @param device Pointer to MTD_NAND device
 * @param page Relative page offset of a logical partition
 * @param data Buffer of main data
 * @param data_len The length you want to write
 * @param spare Buffer of spare data
 * @param spare_len The length you want to write
 *
 * @return RT_MTD_EOK - No error occurs
 *         -RT_MTD_EIO - Programming fail
 */
static rt_err_t nandflash_writepage(struct rt_mtd_nand_device *device,
                             rt_off_t                   page,
                             const rt_uint8_t          *data,
                             rt_uint32_t                data_len,
                             const rt_uint8_t          *spare,
                             rt_uint32_t                spare_len)
{
    return RT_EOK;
}

/**
 * This function erase a block
 *
 * @param device Pointer to MTD_NAND device
 * @param block Relative block offset of a logical partition
 *
 * @return RT_MTD_EOK - Erase successfully
 *         -RT_MTD_EIO - Erase fail
 */
static rt_err_t nandflash_eraseblock(struct rt_mtd_nand_device *device,
                                     rt_uint32_t                block)
{
    return RT_EOK;
}

static rt_err_t nandflash_checkblock(struct rt_mtd_nand_device *device,
                                     rt_uint32_t                block)
{
    return (RT_EOK);
}

static rt_err_t nandflash_mark_badblock(struct rt_mtd_nand_device *device,
                                        rt_uint32_t                block)
{
    return (RT_EOK);
}

static const struct rt_mtd_nand_driver_ops ops =
{
    nandflash_readid,
    nandflash_readpage,
    nandflash_writepage,
    RT_NULL,               /* nandflash_pagecopy, */
    nandflash_eraseblock,
    nandflash_checkblock,
    nandflash_mark_badblock,
};

/**
 * This function initialize and register the MTD NAND device
 */
void rt_hw_mtd_nand_init(void)
{
//    efm32gg_ebi_init();

//    DMA_CfgChannel(EFM32GG_EBI_DMACH, (void *)&chnCfg);
//    rt_mutex_init(&_nand.lock, "nand", RT_IPC_FLAG_FIFO);
//    rt_memset(&_nand.id, 0x00, 5);

    _partition[0].page_size       = PAGE_DATA_SIZE;
    _partition[0].pages_per_block = PAGES_PER_BLOCK;
    _partition[0].block_total     = 512; //2048
    _partition[0].oob_size        = PAGE_OOB_SIZE;
    _partition[0].oob_free        = 60;
    _partition[0].block_start     = 0;
    _partition[0].block_end       = 255;
    _partition[0].ops             = &ops;

    rt_mtd_nand_register_device("nand0", &_partition[0]);
}

