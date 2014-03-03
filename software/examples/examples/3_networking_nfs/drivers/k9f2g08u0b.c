/*
 * File      : k9f2g08u0b.c
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
 * 2013-02-28     heyuanjie87      use dma transfer
 */
#include <rtdevice.h>
#include "board.h"
#include "k9f2g08u0b.h"


#define NAND_DEBUG    rt_kprintf
//#define NAND_DEBUG(...)

#define DMA_CHANNEL     DMA_Channel_0
#define DMA_STREAM      DMA2_Stream0
#define DMA_TCIF        DMA_FLAG_TCIF0
#define DMA_IRQN        DMA2_Stream0_IRQn
#define DMA_IRQ_HANDLER DMA2_Stream0_IRQHandler
#define DMA_CLK         RCC_AHB1Periph_DMA2

#define NAND_BANK     ((rt_uint32_t)0x80000000)
static struct stm32f4_nand _device;

#define ECC_SIZE     4

rt_inline void nand_cmd(rt_uint8_t cmd)
{
    /* write to CMD area */
    *(volatile rt_uint8_t*)(NAND_BANK | CMD_AREA) = cmd;
}

rt_inline void nand_addr(rt_uint8_t addr)
{
    /* write to address area */
    *(volatile rt_uint8_t*)(NAND_BANK | ADDR_AREA) = addr;
}

rt_inline rt_uint8_t nand_read8(void)
{
    /* read 1Byte */
    return (*(volatile rt_uint8_t*)(NAND_BANK | DATA_AREA));
}

rt_inline void nand_write8(rt_uint8_t data)
{
    /* write 1Byte */
    *(volatile rt_uint8_t*)(NAND_BANK | DATA_AREA) = data;
}

rt_inline void nand_waitready(void)
{
    while (GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0);
}

static rt_uint8_t nand_readstatus(void)
{
    nand_cmd(NAND_CMD_STATUS);
    return (nand_read8());
}

static void dmaRead(rt_uint8_t *dst, rt_size_t size)
{
    DMA_InitTypeDef  DMA_InitStructure;

    DMA_InitStructure.DMA_Channel = DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(NAND_BANK | DATA_AREA);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dst;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
    DMA_InitStructure.DMA_BufferSize = size; /* assert_param(0~64K) */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA_STREAM, &DMA_InitStructure);

    DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA_STREAM, DMA_TCIF);
    rt_completion_init(&_device.comp);
    DMA_Cmd(DMA_STREAM, ENABLE);

    if (rt_completion_wait(&_device.comp, 100) != RT_EOK)
    {
        NAND_DEBUG("nand read timeout\n");
    }
}

static void dmaWrite(const rt_uint8_t *src, rt_size_t size)
{
    DMA_InitTypeDef  DMA_InitStructure;

    DMA_InitStructure.DMA_Channel = DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)src;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(NAND_BANK | DATA_AREA);
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
    DMA_InitStructure.DMA_BufferSize = size; /* assert_param(0~64K) */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    DMA_Init(DMA_STREAM, &DMA_InitStructure);

    DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);
    DMA_ClearFlag(DMA_STREAM, DMA_TCIF);
    rt_completion_init(&_device.comp);
    DMA_Cmd(DMA_STREAM, ENABLE);

    if (rt_completion_wait(&_device.comp, 100) != RT_EOK)
    {
        NAND_DEBUG("nand write timeout\n");
    }
}

void DMA_IRQ_HANDLER(void)
{
    DMA_ClearFlag(DMA_STREAM, DMA_TCIF);
    rt_completion_done(&_device.comp);
}

static rt_err_t nand_datacorrect(uint32_t generatedEcc, uint32_t readEcc, uint8_t *data)
{
#define ECC_MASK28    0x0FFFFFFF          /* 28 valid ECC parity bits. */
#define ECC_MASK      0x05555555          /* 14 ECC parity bits.       */

    rt_uint32_t count, bitNum, byteAddr;
    rt_uint32_t mask;
    rt_uint32_t syndrome;
    rt_uint32_t eccP;                            /* 14 even ECC parity bits. */
    rt_uint32_t eccPn;                           /* 14 odd ECC parity bits.  */

    syndrome = (generatedEcc ^ readEcc) & ECC_MASK28;

    if (syndrome == 0)
        return (RT_MTD_EOK);                  /* No errors in data. */

    eccPn = syndrome & ECC_MASK;              /* Get 14 odd parity bits.  */
    eccP  = (syndrome >> 1) & ECC_MASK;       /* Get 14 even parity bits. */

    if ((eccPn ^ eccP) == ECC_MASK)           /* 1-bit correctable error ? */
    {
        bitNum = (eccP & 0x01) |
                 ((eccP >> 1) & 0x02) |
                 ((eccP >> 2) & 0x04);
        NAND_DEBUG("ECC bit %d\n",bitNum);
        byteAddr = ((eccP >> 6) & 0x001) |
                   ((eccP >> 7) & 0x002) |
                   ((eccP >> 8) & 0x004) |
                   ((eccP >> 9) & 0x008) |
                   ((eccP >> 10) & 0x010) |
                   ((eccP >> 11) & 0x020) |
                   ((eccP >> 12) & 0x040) |
                   ((eccP >> 13) & 0x080) |
                   ((eccP >> 14) & 0x100) |
                   ((eccP >> 15) & 0x200) |
                   ((eccP >> 16) & 0x400) ;

        data[ byteAddr ] ^= 1 << bitNum;

        return RT_MTD_EOK;
    }

    /* Count number of one's in the syndrome. */
    count = 0;
    mask  = 0x00800000;
    while (mask)
    {
        if (syndrome & mask)
            count++;
        mask >>= 1;
    }

    if (count == 1)           /* Error in the ECC itself. */
        return RT_MTD_EECC;

    return -RT_MTD_EECC;       /* Unable to correct data. */

#undef ECC_MASK
#undef ECC_MASK24
}

static void gpio_nandflash_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

    /* FSMC GPIO config in board.c */

    /* NAND_R/B: PG6 */
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void fsmc_nandflash_init(void)
{
    FSMC_NANDInitTypeDef FSMC_NANDInitStructure;
    FSMC_NAND_PCCARDTimingInitTypeDef p;

    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);

    p.FSMC_SetupTime     = 0x1;
    p.FSMC_WaitSetupTime = 0x3;
    p.FSMC_HoldSetupTime = 0x2;
    p.FSMC_HiZSetupTime  = 0x1;

    FSMC_NANDInitStructure.FSMC_Bank = FSMC_Bank3_NAND;
    FSMC_NANDInitStructure.FSMC_Waitfeature = FSMC_Waitfeature_Disable;
    FSMC_NANDInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStructure.FSMC_ECC = FSMC_ECC_Disable;/* 先不要开启ECC计算 */
    FSMC_NANDInitStructure.FSMC_ECCPageSize = FSMC_ECCPageSize_2048Bytes;
    FSMC_NANDInitStructure.FSMC_TCLRSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_TARSetupTime = 0x00;
    FSMC_NANDInitStructure.FSMC_CommonSpaceTimingStruct = &p;
    FSMC_NANDInitStructure.FSMC_AttributeSpaceTimingStruct = &p;

    FSMC_NANDInit(&FSMC_NANDInitStructure);

    /* FSMC NAND Bank Cmd Test */
    FSMC_NANDCmd(FSMC_Bank3_NAND, ENABLE);
}

static rt_err_t nandflash_readid(struct rt_mtd_nand_device *mtd)
{
    nand_cmd(NAND_CMD_READID);
    nand_addr(0);

    _device.id[0] = nand_read8();
    _device.id[1] = nand_read8();
    _device.id[2] = nand_read8();
    _device.id[3] = nand_read8();
    NAND_DEBUG("ID[%X,%X]\n",_device.id[0], _device.id[1]);
    if (_device.id[0] == 0xEC && _device.id[1] == 0xDA)
    {
        return (RT_EOK);
    }

    return (RT_ERROR);
}

static rt_err_t nandflash_readpage(struct rt_mtd_nand_device* device, rt_off_t page,
                                   rt_uint8_t *data, rt_uint32_t data_len,
                                   rt_uint8_t *spare, rt_uint32_t spare_len)
{
    rt_uint32_t index;
    rt_uint32_t gecc, recc;
    rt_uint8_t tmp[4];
    rt_err_t result;

    page = page + device->block_start * device->pages_per_block;
    if (page/device->pages_per_block > device->block_end)
    {
        return -RT_MTD_EIO;
    }

    result = RT_MTD_EOK;
    rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);

    if (data && data_len)
    {
        nand_cmd(NAND_CMD_READ_1);
        nand_addr(0);
        nand_addr(0);
        nand_addr(page);
        nand_addr(page >> 8);
        nand_addr(page >> 16);
        nand_cmd(NAND_CMD_READ_TRUE);

        nand_waitready();

        FSMC_NANDECCCmd(FSMC_Bank3_NAND,ENABLE);
        dmaRead(data, data_len);
        gecc = FSMC_GetECC(FSMC_Bank3_NAND);
        FSMC_NANDECCCmd(FSMC_Bank3_NAND,DISABLE);

        if (data_len == PAGE_DATA_SIZE)
        {
            for (index = 0; index < ECC_SIZE; index ++)
                tmp[index] = nand_read8();
            if (spare && spare_len)
            {
                dmaRead(&spare[ECC_SIZE], spare_len - ECC_SIZE);
                rt_memcpy(spare, tmp , ECC_SIZE);
            }

            recc   = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];

            if (recc != 0xFFFFFFFF && gecc != 0)
                result = nand_datacorrect(gecc, recc, data);

            if (result != RT_MTD_EOK)
                NAND_DEBUG("page: %d, gecc %X, recc %X>",page, gecc, recc);

            goto _exit;
        }
    }

    if (spare && spare_len)
    {
        nand_cmd(NAND_CMD_READ_1);
        nand_addr(0);
        nand_addr(8);
        nand_addr(page);
        nand_addr(page >> 8);
        nand_addr(page >> 16);
        nand_cmd(NAND_CMD_READ_TRUE);

        nand_waitready();

        dmaRead(spare, spare_len);
    }
_exit:
    rt_mutex_release(&_device.lock);

    return (result);
}

static rt_err_t nandflash_writepage(struct rt_mtd_nand_device* device, rt_off_t page,
                                    const rt_uint8_t *data, rt_uint32_t data_len,
                                    const rt_uint8_t *spare, rt_uint32_t spare_len)
{
    rt_err_t result;
    rt_uint32_t gecc;

    page = page + device->block_start * device->pages_per_block;
    if (page/device->pages_per_block > device->block_end)
    {
        return -RT_MTD_EIO;
    }

    result = RT_MTD_EOK;
    rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);

    if (data && data_len)
    {
        nand_cmd(NAND_CMD_PAGEPROGRAM);

        nand_addr(0);
        nand_addr(0);
        nand_addr(page);
        nand_addr(page >> 8);
        nand_addr(page >> 16);

        FSMC_NANDECCCmd(FSMC_Bank3_NAND,ENABLE);
        dmaWrite(data, data_len);
        gecc = FSMC_GetECC(FSMC_Bank3_NAND);
        FSMC_NANDECCCmd(FSMC_Bank3_NAND,DISABLE);

        if (data_len == PAGE_DATA_SIZE)
        {
            nand_write8((uint8_t)gecc);
            nand_write8((uint8_t)(gecc >> 8));
            nand_write8((uint8_t)(gecc >> 16));
            nand_write8((uint8_t)(gecc >> 24));

            if (spare && spare_len)
                dmaWrite(&spare[ECC_SIZE], spare_len - ECC_SIZE);
        }

        nand_cmd(NAND_CMD_PAGEPROGRAM_TRUE);

        nand_waitready();

        if (nand_readstatus() & 0x01 == 1)
            result = -RT_MTD_EIO;
        goto _exit;
    }

    if (spare && spare_len)
    {
        nand_cmd(NAND_CMD_PAGEPROGRAM);

        nand_addr(ECC_SIZE);
        nand_addr(0x08);
        nand_addr(page);
        nand_addr(page >> 8);
        nand_addr(page >> 16);

        dmaWrite(&spare[ECC_SIZE], spare_len - ECC_SIZE);

        nand_cmd(NAND_CMD_PAGEPROGRAM_TRUE);
        nand_waitready();

        if (nand_readstatus() & 0x01 == 1)
            result = -RT_MTD_EIO;
    }

_exit:
    rt_mutex_release(&_device.lock);

    return (result);
}

rt_err_t nandflash_eraseblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    rt_uint32_t page;
    rt_err_t result;

    block = block + device->block_start;

    result = RT_MTD_EOK;
    page = block * 64;

    rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);

    nand_cmd(NAND_CMD_ERASE0);

    nand_addr(page);
    nand_addr(page >> 8);
    nand_addr(page >> 16);

    nand_cmd(NAND_CMD_ERASE1);

    nand_waitready();

    if (nand_readstatus() & 0x01 == 1)
        result = -RT_MTD_EIO;
    rt_mutex_release(&_device.lock);

    return (result);
}

static rt_err_t nandflash_pagecopy(struct rt_mtd_nand_device *device, rt_off_t src_page, rt_off_t dst_page)
{
    rt_err_t result;

    src_page = src_page + device->block_start * device->pages_per_block;
    dst_page = dst_page + device->block_start * device->pages_per_block;

    result = RT_MTD_EOK;
    rt_mutex_take(&_device.lock, RT_WAITING_FOREVER);

    nand_cmd(NAND_CMD_RDCOPYBACK);

    nand_addr(0);
    nand_addr(0);
    nand_addr(src_page);
    nand_addr(src_page >> 8);
    nand_addr(src_page >> 16);

    nand_cmd(NAND_CMD_RDCOPYBACK_TRUE);

    nand_waitready();

    nand_cmd(NAND_CMD_COPYBACKPGM);

    nand_addr(0);
    nand_addr(0);
    nand_addr(dst_page);
    nand_addr(dst_page >> 8);
    nand_addr(dst_page >> 16);

    nand_cmd(NAND_CMD_COPYBACKPGM_TRUE);

    nand_waitready();
    if ((nand_readstatus() & 0x01) == 0x01)
        result = -RT_MTD_EIO;

    rt_mutex_release(&_device.lock);

    return (result);
}

static rt_err_t nandflash_checkblock(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_MTD_EOK);
}

static rt_err_t nandflash_markbad(struct rt_mtd_nand_device* device, rt_uint32_t block)
{
    return (RT_MTD_EOK);
}

static struct rt_mtd_nand_driver_ops ops =
{
    nandflash_readid,
    nandflash_readpage,
    nandflash_writepage,
    nandflash_pagecopy,
    nandflash_eraseblock,
#if defined(RT_USING_DFS_UFFS) && !defined(RT_UFFS_USE_CHECK_MARK_FUNCITON)
    RT_NULL,
    RT_NULL,
#else
    nandflash_checkblock,
    nandflash_markbad
#endif
};

static struct rt_mtd_nand_device _partition[2];

void rt_hw_mtd_nand_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = DMA_IRQN;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    gpio_nandflash_init();
    fsmc_nandflash_init();
    RCC_AHB1PeriphClockCmd(DMA_CLK, ENABLE);
    NVIC_Init(&NVIC_InitStructure);

    rt_mutex_init(&_device.lock, "nand", RT_IPC_FLAG_FIFO);

    _partition[0].page_size   = 2048;
    _partition[0].pages_per_block = 64;
    _partition[0].block_total = 256;
    _partition[0].oob_size    = 64;
    _partition[0].oob_free    = 60;
    _partition[0].block_start = 0;
    _partition[0].block_end   = 255;
    _partition[0].ops         = &ops;

    rt_mtd_nand_register_device("nand0", &_partition[0]);

    _partition[1].page_size   = NAND_PAGE_SIZE;
    _partition[1].pages_per_block = 64;
    _partition[1].block_total = 2048 - _partition[0].block_total;
    _partition[1].oob_size    = 64;
    _partition[1].oob_free    = 60;
    _partition[1].block_start = _partition[0].block_end + 1;
    _partition[1].block_end   = 2047;
    _partition[1].ops         = &ops;

    rt_mtd_nand_register_device("nand1", &_partition[1]);
}

#include <finsh.h>
void nread(rt_uint32_t partion, rt_uint32_t page)
{
    int i;
    rt_uint8_t spare[64];
    rt_uint8_t *data_ptr;
    struct rt_mtd_nand_device *device;

    if (partion >= 3)
        return;
    device = &_partition[partion];
    data_ptr = (rt_uint8_t*) rt_malloc(PAGE_DATA_SIZE);
    if (data_ptr == RT_NULL)
    {
        rt_kprintf("no memory\n");
        return;
    }

    rt_memset(spare, 0, sizeof(spare));
    rt_memset(data_ptr, 0, PAGE_DATA_SIZE);
    nandflash_readpage(device, page, data_ptr, PAGE_DATA_SIZE, spare, sizeof(spare));
    for (i = 0; i < 512; i ++)
    {
        rt_kprintf("0x%X,",data_ptr[i]);
    }

    rt_kprintf("\n spare\n");
    for (i = 0; i < sizeof(spare); i ++)
    {
        rt_kprintf("0x%X,",spare[i]);
    }
    rt_kprintf("\n\n");

    /* release memory */
    rt_free(data_ptr);
}

void nwrite(rt_uint32_t partion, int page)
{
    int i;
    rt_uint8_t spare[64];
    rt_uint8_t *data_ptr;
    struct rt_mtd_nand_device *device;

    if (partion >= 3)
        return;
    device = &_partition[partion];

    data_ptr = (rt_uint8_t*) rt_malloc (PAGE_DATA_SIZE);
    if (data_ptr == RT_NULL)
    {
        rt_kprintf("no memory.\n");
        return;
    }

    /* Need random data to test ECC */
    for (i = 0; i < PAGE_DATA_SIZE; i ++)
        data_ptr[i] = i/5 -i;
    rt_memset(spare, 0xdd, sizeof(spare));
    nandflash_writepage(device, page, data_ptr, PAGE_DATA_SIZE, spare, sizeof(spare));

    rt_free(data_ptr);
}

void ncopy(rt_uint32_t partion, int src, int dst)
{
    struct rt_mtd_nand_device *device;

    if (partion >= 3)
        return;
    device = &_partition[partion];
    nandflash_pagecopy(device, src, dst);
}

void nerase(int partion, int block)
{
    struct rt_mtd_nand_device *device;

    if (partion >= 3)
        return;
    device = &_partition[partion];
    nandflash_eraseblock(device, block);
}

void nerase_all(rt_uint32_t partion)
{
    rt_uint32_t index;
    struct rt_mtd_nand_device *device;

    if (partion >= 3)
        return;
    device = &_partition[partion];
    for (index = 0; index < device->block_total; index ++)
    {
        nandflash_eraseblock(device, index);
    }
}

void nid(void)
{
    nandflash_readid(0);
}

FINSH_FUNCTION_EXPORT(nid, nand id);
FINSH_FUNCTION_EXPORT(ncopy, nand copy page);
FINSH_FUNCTION_EXPORT(nerase, nand erase a block of one partiton);
FINSH_FUNCTION_EXPORT(nerase_all, erase all blocks of a partition);
FINSH_FUNCTION_EXPORT(nwrite, nand write page);
FINSH_FUNCTION_EXPORT(nread, nand read page);
