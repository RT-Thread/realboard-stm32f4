#include <finsh.h>
#include "board.h"

static void rd8(void)
{
    volatile char * p = (volatile char *)STM32_EXT_SRAM_BEGIN;
    char value = *p;
    value = *p;
    value = *p;
    value = *p;
}
FINSH_FUNCTION_EXPORT(rd8, read 8bit);

static void rd16(void)
{
    volatile short * p = (volatile short *)STM32_EXT_SRAM_BEGIN;
    short value = *p;
    value = *p;
    value = *p;
    value = *p;
}
FINSH_FUNCTION_EXPORT(rd16, read 16bit);

static void rd32(void)
{
    volatile int * p = (volatile int *)STM32_EXT_SRAM_BEGIN;
    int value = *p;
    value = *p;
    value = *p;
    value = *p;
}
FINSH_FUNCTION_EXPORT(rd32, read 32bit);

static void wr8(void)
{
    volatile char * p = (volatile char *)STM32_EXT_SRAM_BEGIN;
    char value = 0xAA;
    *p = value;
    *p = value;
    *p = value;
    *p = value;
}
FINSH_FUNCTION_EXPORT(wr8, write 8bit);

static void wr16(void)
{
    volatile short * p = (volatile short *)STM32_EXT_SRAM_BEGIN;
    short value = 0xAAAA;
    *p = value;
    *p = value;
    *p = value;
    *p = value;
}
FINSH_FUNCTION_EXPORT(wr16, write 16bit);

static void wr32(void)
{
    volatile int * p = (volatile int *)STM32_EXT_SRAM_BEGIN;
    int value = 0xAAAAAAAA;
    *p = value;
    *p = value;
    *p = value;
    *p = value;
}
FINSH_FUNCTION_EXPORT(wr32, write 32bit);

static int sram_buffer[128];
static void dma_init(void)
{
  /* Enable the DMA2 Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}
FINSH_FUNCTION_EXPORT(dma_init, dma_init);

static dma_rd16(void)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(DMA2_Stream3, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(DMA2_Stream3);

//  /* Write to FSMC -----------------------------------------------------------*/
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sram_buffer;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 128;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_VeryHigh DMA_Priority_High
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
//  DMA_Init(DMA2_Channel5, &DMA_InitStructure);
//
//  /* Enable DMA2 channel5 */
//  DMA_Cmd(DMA2_Channel5, ENABLE);

  SDDMA_InitStructure.DMA_Channel = DMA_Channel_4;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sram_buffer;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  SDDMA_InitStructure.DMA_BufferSize = 128; /* assert_param(0~64K) */
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA2_Stream3, &SDDMA_InitStructure);

  DMA_FlowControllerConfig(DMA2_Stream3, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(DMA2_Stream3, ENABLE);

//  /* Check if DMA2 channel5 transfer is finished */
//  while(!DMA_GetFlagStatus(DMA2_FLAG_TC5));
}
FINSH_FUNCTION_EXPORT(dma_rd16, DMA read 16bit);

static dma_rd32(void)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(DMA2_Stream3, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(DMA2_Stream3);

//  /* Write to FSMC -----------------------------------------------------------*/
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sram_buffer;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 128;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_VeryHigh DMA_Priority_High
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
//  DMA_Init(DMA2_Channel5, &DMA_InitStructure);
//
//  /* Enable DMA2 channel5 */
//  DMA_Cmd(DMA2_Channel5, ENABLE);

  SDDMA_InitStructure.DMA_Channel = DMA_Channel_4;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)sram_buffer;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  SDDMA_InitStructure.DMA_BufferSize = 128; /* assert_param(0~64K) */
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA2_Stream3, &SDDMA_InitStructure);

  DMA_FlowControllerConfig(DMA2_Stream3, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(DMA2_Stream3, ENABLE);

//  /* Check if DMA2 channel5 transfer is finished */
//  while(!DMA_GetFlagStatus(DMA2_FLAG_TC5));
}
FINSH_FUNCTION_EXPORT(dma_rd32, DMA read 32bit);

static dma_wr16(void)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(DMA2_Stream3, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(DMA2_Stream3);

//  /* Write to FSMC -----------------------------------------------------------*/
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sram_buffer;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 128;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_VeryHigh DMA_Priority_High
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
//  DMA_Init(DMA2_Channel5, &DMA_InitStructure);
//
//  /* Enable DMA2 channel5 */
//  DMA_Cmd(DMA2_Channel5, ENABLE);

  SDDMA_InitStructure.DMA_Channel = DMA_Channel_4;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sram_buffer;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  SDDMA_InitStructure.DMA_BufferSize = 128; /* assert_param(0~64K) */
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA2_Stream3, &SDDMA_InitStructure);

  DMA_FlowControllerConfig(DMA2_Stream3, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(DMA2_Stream3, ENABLE);

//  /* Check if DMA2 channel5 transfer is finished */
//  while(!DMA_GetFlagStatus(DMA2_FLAG_TC5));
}
FINSH_FUNCTION_EXPORT(dma_wr16, DMA write 16bit);

static dma_wr32(void)
{
  DMA_InitTypeDef SDDMA_InitStructure;

  DMA_ClearFlag(DMA2_Stream3, DMA_FLAG_FEIF3 | DMA_FLAG_DMEIF3 | DMA_FLAG_TEIF3 | DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);

  /* DMA2 Stream3  or Stream6 disable */
  DMA_Cmd(DMA2_Stream3, DISABLE);

  /* DMA2 Stream3  or Stream6 Config */
  DMA_DeInit(DMA2_Stream3);

//  /* Write to FSMC -----------------------------------------------------------*/
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)sram_buffer;
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//  DMA_InitStructure.DMA_BufferSize = 128;
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//DMA_Priority_VeryHigh DMA_Priority_High
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
//  DMA_Init(DMA2_Channel5, &DMA_InitStructure);
//
//  /* Enable DMA2 channel5 */
//  DMA_Cmd(DMA2_Channel5, ENABLE);

  SDDMA_InitStructure.DMA_Channel = DMA_Channel_4;
  SDDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sram_buffer;
  SDDMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)STM32_EXT_SRAM_BEGIN;
  SDDMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  SDDMA_InitStructure.DMA_BufferSize = 128; /* assert_param(0~64K) */
  SDDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  SDDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  SDDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  SDDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  SDDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  SDDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  SDDMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  SDDMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  SDDMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  SDDMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA2_Stream3, &SDDMA_InitStructure);

  DMA_FlowControllerConfig(DMA2_Stream3, DMA_FlowCtrl_Peripheral);

  /* DMA2 Stream3  or Stream6 enable */
  DMA_Cmd(DMA2_Stream3, ENABLE);

//  /* Check if DMA2 channel5 transfer is finished */
//  while(!DMA_GetFlagStatus(DMA2_FLAG_TC5));
}
FINSH_FUNCTION_EXPORT(dma_wr32, DMA wite 32bit);

