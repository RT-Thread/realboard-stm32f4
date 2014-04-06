#include <rtthread.h>
#include "board.h"

#ifdef RT_USING_RTC
#include "stm32f4_rtc.h"
#endif /* RT_USING_RTC */

#ifdef RT_USING_SPI
#include "stm32f20x_40x_spi.h"
#include "spi_flash_w25qxx.h"

/*
SPI2_MOSI: PB15
SPI2_MISO: PB14
SPI2_SCK : PB13

CS0: PG10  SPI FLASH
CS1: PB12  TOUCH
CS2: PG7   WIFI
*/
static void rt_hw_spi2_init(void)
{
    static struct stm32_spi_bus stm32_spi;
    GPIO_InitTypeDef GPIO_InitStructure;

    static struct rt_spi_device spi_device;
    static struct stm32_spi_cs  spi_cs;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* register spi bus */
    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        /*!< SPI SCK pin configuration */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        /* Connect alternate function */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }

    {
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &GPIO_InitStructure);

        GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_SPI1);

        stm32_spi_register(SPI2, &stm32_spi, "spi1");
    }

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;

    {
        spi_cs.GPIOx = GPIOA;
        spi_cs.GPIO_Pin = GPIO_Pin_4;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

	// TODO: RT_SPI_MODE_3 | RT_SPI_MSB, 30 MHz?

        rt_spi_bus_attach_device(&spi_device, "spi10", "spi1", (void*)&spi_cs);
    }

    /* attach cs */
    {
        /* spi20: PG10 */
        spi_cs.GPIOx = GPIOG;
        spi_cs.GPIO_Pin = GPIO_Pin_10;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi20", "spi2", (void*)&spi_cs);
    }

    /* attach cs */
    {
        /* spi21: PB12 */
        spi_cs.GPIOx = GPIOB;
        spi_cs.GPIO_Pin = GPIO_Pin_12;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi21", "spi2", (void*)&spi_cs);
    }

    /* attach cs */
    {
        /* spi20: PG7 */
        spi_cs.GPIOx = GPIOG;
        spi_cs.GPIO_Pin = GPIO_Pin_7;
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "spi22", "spi2", (void*)&spi_cs);
    }
}
#endif /* RT_USING_SPI */

void rt_platform_init(void)
{
#ifdef RT_USING_SPI
    rt_hw_spi2_init();

#ifdef RT_USING_DFS
    w25qxx_init("flash0", "spi20");
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize touch panel */
    rtgui_touch_hw_init("spi21");
#endif /* RT_USING_RTGUI */
#endif /* RT_USING_SPI */

#ifdef RT_USING_USB_HOST
    /* register stm32 usb host controller driver */
    rt_hw_susb_init();
#endif

#ifdef RT_USING_DFS
    /* initilize sd card */
#ifdef RT_USING_SDIO
    rt_mmcsd_core_init();
    rt_mmcsd_blk_init();
    stm32f4xx_sdio_init();
    rt_thread_delay(RT_TICK_PER_SECOND);
#else
    rt_hw_sdcard_init();
#endif
#endif /* RT_USING_DFS */

#ifdef RT_USING_RTGUI
    /* initilize ra8875 lcd controller */
    ra8875_init();

    /* initilize key module */
    rt_hw_key_init();
#endif /* RT_USING_RTGUI */

#ifdef RT_USING_RTC
    rt_hw_rtc_init();
#endif /* RT_USING_RTC */

    rt_thread_delay(50);
    rt_device_init_all();
}

