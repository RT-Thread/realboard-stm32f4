#ifndef __STM32F4XX_SD_H__
#define __STM32F4XX_SD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

//#define SD_POLLING_MODE
    /**
      * @brief  SD FLASH SDIO Interface
      */
#define SD_DETECT_PIN                    GPIO_Pin_13                 /* PH.13 */
#define SD_DETECT_GPIO_PORT              GPIOH                       /* GPIOH */
#define SD_DETECT_GPIO_CLK               RCC_AHB1Periph_GPIOH

#define SDIO_FIFO_ADDRESS                ((uint32_t)0x40012C80)
    /**
      * @brief  SDIO Intialization Frequency (400KHz max)
      */
#define SDIO_INIT_CLK_DIV                ((uint8_t)0x76)
    /**
      * @brief  SDIO Data Transfer Frequency (25MHz max)
      */
#define SDIO_TRANSFER_CLK_DIV            ((uint8_t)0x0)

#define SD_SDIO_DMA                   DMA2
#define SD_SDIO_DMA_CLK               RCC_AHB1Periph_DMA2

#define SD_SDIO_DMA_STREAM3           3
//#define SD_SDIO_DMA_STREAM6           6

#ifdef SD_SDIO_DMA_STREAM3
#define SD_SDIO_DMA_STREAM            DMA2_Stream3
#define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
#define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF3
#define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF3
#define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF3
#define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF3
#define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF3
#define SD_SDIO_DMA_IRQn              DMA2_Stream3_IRQn
#define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream3_IRQHandler
#elif defined SD_SDIO_DMA_STREAM6
#define SD_SDIO_DMA_STREAM            DMA2_Stream6
#define SD_SDIO_DMA_CHANNEL           DMA_Channel_4
#define SD_SDIO_DMA_FLAG_FEIF         DMA_FLAG_FEIF6
#define SD_SDIO_DMA_FLAG_DMEIF        DMA_FLAG_DMEIF6
#define SD_SDIO_DMA_FLAG_TEIF         DMA_FLAG_TEIF6
#define SD_SDIO_DMA_FLAG_HTIF         DMA_FLAG_HTIF6
#define SD_SDIO_DMA_FLAG_TCIF         DMA_FLAG_TCIF6
#define SD_SDIO_DMA_IRQn              DMA2_Stream6_IRQn
#define SD_SDIO_DMA_IRQHANDLER        DMA2_Stream6_IRQHandler
#endif /* SD_SDIO_DMA_STREAM3 */

    /**
      * @brief  SD detection on its memory slot
      */
#define SD_PRESENT                                 ((uint8_t)0x01)
#define SD_NOT_PRESENT                             ((uint8_t)0x00)

#ifdef __cplusplus
}
#endif

#endif
