#include <rtthread.h>
#include <board.h>

#ifdef RT_USING_I2C_BITOPS

#include <i2c.h>
#include <i2c-bit-ops.h>

/* GPIO define */
#define GPIO_PORT_I2C	GPIOB
#define RCC_I2C_PORT 	RCC_AHB1Periph_GPIOB
#define I2C_SCL_PIN		GPIO_Pin_6
#define I2C_SDA_PIN		GPIO_Pin_7

static struct rt_i2c_bus_device i2c_device;

static void gpio_set_sda(void *data, rt_int32_t state)
{
	if (state)
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN);
	}
	else
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN);
	}
}

static void gpio_set_scl(void *data, rt_int32_t state)
{
	if (state)
	{
		GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN);
	}
	else
	{
		GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN);
	}
}

static rt_int32_t gpio_get_sda(void *data)
{
	return GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN);
}

static rt_int32_t gpio_get_scl(void *data)
{
	return GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SCL_PIN);
}

static void gpio_udelay(rt_uint32_t us)
{
	volatile rt_int32_t i;
	for (; us > 0; us--)
	{
		i = 50000;
		while(i > 0)
		{
			i--;
		}
	}
}

static const struct rt_i2c_bit_ops bit_ops =
{
	RT_NULL,
	gpio_set_sda,
	gpio_set_scl,
	gpio_get_sda,
	gpio_get_scl,

	gpio_udelay,

	5,
	100
};

#else
static rt_err_t stm32_i2c_configure(struct rt_i2c_bus_device * device,
                                    struct rt_i2c_configuration* configuration)
{
    return RT_EOK;
}

static rt_err_t stm32_i2c_start(struct rt_i2c_bus_device * device)
{
//    /* Send START condition */
//    I2C_GenerateSTART(I2C1, ENABLE);
//
//    /* Test on EV5 and clear it */
//    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    return RT_EOK;
}

static rt_err_t stm32_i2c_stop(struct rt_i2c_bus_device * device)
{
//    /* Send STOP Condition */
//    I2C_GenerateSTOP(I2C1, ENABLE);
//
    return RT_EOK;
}

static rt_size_t stm32_i2c_read(struct rt_i2c_bus_device * device,
                                struct rt_i2c_message * message,
                                rt_uint8_t * read_buffer,
                                rt_size_t size)
{
	uint16_t temp;

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send EEPROM address for write */
	I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C1, *message->device_offset);

	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send STRAT condition a second time */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send EEPROM address for read */
	I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Receiver);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));


	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

	I2C_AcknowledgeConfig(I2C1,DISABLE);

	*read_buffer++ = I2C_ReceiveData(I2C1);

	/* Test on EV7 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));

	*read_buffer++ |= I2C_ReceiveData(I2C1);

	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	I2C_AcknowledgeConfig(I2C1,ENABLE);

    return size;
}

static rt_size_t stm32_i2c_write(struct rt_i2c_bus_device * device,
                                 struct rt_i2c_message * message,
                                 const rt_uint8_t * write_buffer,
                                 rt_size_t size)
{
	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	/* Send EEPROM address for write */
	I2C_Send7bitAddress(I2C1, message->device_addr, I2C_Direction_Transmitter);

	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	/* Send the EEPROM's internal address to write to */
	I2C_SendData(I2C1, *message->device_offset);

	/* Test on EV8 and clear it */
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send the current byte */
	I2C_SendData(I2C1, (uint8_t)*write_buffer++);

	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send the current byte */
	I2C_SendData(I2C1, (uint8_t)*write_buffer++);

	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	/* Send STOP condition */
	I2C_GenerateSTOP(I2C1, ENABLE);

	return 0;
}

struct rt_i2c_bus_device stm32_i2c1;
const struct rt_i2c_ops stm32_i2c_ops =
{
    stm32_i2c_configure,
    stm32_i2c_start,
    stm32_i2c_stop,
    stm32_i2c_read,
    stm32_i2c_write,
};
#endif

void rt_hw_i2c_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

#ifdef RT_USING_I2C_BITOPS
    RCC_AHB1PeriphClockCmd(RCC_I2C_PORT, ENABLE);	/* ´ò¿ªGPIOÊ±ÖÓ */

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN | I2C_SDA_PIN;
    GPIO_Init(GPIO_PORT_I2C, &GPIO_InitStructure);

    rt_memset((void *)&i2c_device, 0, sizeof(struct rt_i2c_bus_device));
    i2c_device.priv = (void *)&bit_ops;
    rt_i2c_bit_add_bus(&i2c_device, "i2c1");

#else
    I2C_InitTypeDef  I2C_InitStructure;

    /*!< sEE_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Reset sEE_I2C IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);

    /* Release reset signal of sEE_I2C IP */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    /*!< GPIO configuration */
    /*!< Configure sEE_I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!< Configure sEE_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Connect PXx to I2C_SCL*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);

    /* Connect PXx to I2C_SDA*/
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);


    /*!< I2C configuration */
    /* sEE_I2C configuration */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = SLA_ADDRESS;
    I2C_InitStructure.I2C_OwnAddress1 = 0x18;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 400000;

    /* Apply sEE_I2C configuration after enabling it */
    I2C_Init(I2C1, &I2C_InitStructure);
    /* sEE_I2C Peripheral Enable */
    I2C_Cmd(I2C1, ENABLE);

    rt_i2c_bus_register(&stm32_i2c1, "i2c1", &stm32_i2c_ops);
#endif
}
