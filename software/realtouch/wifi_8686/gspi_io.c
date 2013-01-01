#include <string.h>
#include "stm32f4xx.h"
#include "rtthread.h"
#include "gspi_io.h"
#include <drivers/spi.h>

struct rt_spi_device *rt_spi_device;

/*
RESET: PB0
INT:   PB1
*/

int g_dummy_clk_reg = 0;
int g_dummy_clk_ioport = 0;
int g_bus_mode_reg = 0x02;
static struct rt_semaphore gspi_lock;

static void gpio_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIO Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    /* Configure PA1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	rt_thread_delay(1);
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	rt_thread_delay(1);
}

static int gspi_acquire_io(void)
{
	rt_sem_take(&gspi_lock, RT_WAITING_FOREVER);
	return 0;
}

static void gspi_release_io(void)
{
	rt_sem_release(&gspi_lock);
	return;
}

static char transmittembuf[2048];
int gspi_write_data_direct(u8 * data, u16 reg, u16 n)
{
	gspi_acquire_io();

	/* N bytes are sent wrt to 16 bytes, convert it to 8 bytes */
	n = (n * 2);
	reg |= 0x8000;

	rt_memcpy(transmittembuf, &reg, sizeof(u16));
	rt_memcpy(transmittembuf + sizeof(u16), data, (n - 2));
	if ((n % 4) != 0)
	{
		rt_memset(transmittembuf + n, 0x00, 2);
		n = n + 2;
	}

	rt_spi_send(rt_spi_device, transmittembuf, n / 2);

	gspi_release_io();

	return 0;
}

int gspi_write_reg(u16 reg, u16 val)
{
	gspi_write_data_direct((u8 *) &val, reg, 2);

	return 0;
}

static char rectembuf[2048 + 1024];
int gspi_read_data_direct(u8 * data, u16 reg, u16 n)
{
	u16 wlanreg;

	if (gspi_acquire_io())
	{
		return -1;
	}

	n = ((n + g_dummy_clk_ioport) * 2);

	wlanreg = reg;
	rt_spi_send_then_recv(rt_spi_device, &wlanreg, 1, rectembuf, n / 2);

	rt_memcpy(data, rectembuf + (g_dummy_clk_ioport) * 2,
			(n - (g_dummy_clk_ioport) * 2));

	gspi_release_io();
	return 0;
}

int gspi_read_reg(u16 reg, u16 * val)
{
	u16 value[8];

	gspi_read_data_direct((u8 *) value, reg, 2);
	memcpy(val, value, 2);

	return 0;
}

extern void sbi_interrupt(int vector);
/*config GPIO as wlan interrupt */
void EXTI1_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable GPIOB clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PB1 pin as input floating */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect EXTI Line1 to PB1 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

	/* Configure EXTI Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void disable_wlan_interrupt(void)
{
	NVIC_DisableIRQ(EXTI1_IRQn);
}

void enable_wlan_interrupt(void)
{
	NVIC_EnableIRQ(EXTI1_IRQn);
}

extern void wlan_interrupt(void);
void EXTI1_IRQHandler(void)
{
	/* perform wlan interrupt */
	wlan_interrupt();
	disable_wlan_interrupt();

	/* Clear the EXTI0 line pending bit */
	EXTI_ClearITPendingBit(EXTI_Line1);
}

int gspi_register_irq(int * irqnum)
{
	NVIC_DisableIRQ(EXTI1_IRQn);
	EXTI1_Config();
	EXTI_ClearITPendingBit(EXTI_Line1);
	*irqnum = EXTI1_IRQn;
	return GSPI_OK;
}

void gspi_irq_clear(void)
{
	EXTI_ClearITPendingBit(EXTI_Line1);
}

int gspihost_init(const char* spi_device)
{
	int ret = RT_EOK;
	ret = rt_sem_init(&gspi_lock, "wifi_gspi", 1, RT_IPC_FLAG_FIFO);
	if (ret != RT_EOK) return ret;

	rt_spi_device = (struct rt_spi_device *) rt_device_find(spi_device);
	if (rt_spi_device == RT_NULL)
	{
		return -RT_ERROR;
	}

	/* config spi */
	{
		struct rt_spi_configuration cfg;
		cfg.data_width = 16;
		cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB;
		cfg.max_hz = 20 * 1000 * 1000;
		rt_spi_configure(rt_spi_device, &cfg);
	}

	gpio_init();
	return ret;
}
