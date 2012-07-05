#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"
#include "codec_wm8978_i2c.h"

/* CODEC config */
#define CODEC_MASTER_MODE       1 /* 0: mcu-master, 1: codec-master. */

/* CODEC PLL config */
/* MCLK : 25M (PLLPRESCALE) (divide by 2 sets the required) */
#define PLL_N_112896            (7 | PLLPRESCALE)
#define PLL_K_112896            (6254546)
#define PLL_N_122880            (8 | PLLPRESCALE)
#define PLL_K_122880            (416349)

/* CODEC port define */
#define CODEC_I2S_PORT          SPI3
#define CODEC_I2S_IRQ           SPI3_IRQn
#define CODEC_I2S_DMA           DMA2_Channel2
#define CODEC_I2S_DMA_IRQ       DMA2_Channel2_IRQn
#define CODEC_I2S_RCC_APB1      RCC_APB1Periph_SPI3
#define CODEC_I2S_RCC_AHB       RCC_AHBPeriph_DMA2

/* I2S DMA Stream definitions */
#define AUDIO_I2S_DMA_CLOCK     RCC_AHB1Periph_DMA1
#define AUDIO_I2S_DMA_STREAM    DMA1_Stream7
#define AUDIO_I2S_DMA_CHANNEL   DMA_Channel_0
#define AUDIO_I2S_DMA_IRQ       DMA1_Stream7_IRQn
#define AUDIO_I2S_DMA_IT_TC     DMA_IT_TCIF7

void vol(uint16_t v);
static void codec_send(rt_uint16_t s_data);

#define DATA_NODE_MAX 5
/* data node for Tx Mode */
struct codec_data_node
{
    rt_uint16_t *data_ptr;
    rt_size_t  data_size;
};

struct codec_device
{
    /* inherit from rt_device */
    struct rt_device parent;

    /* pcm data list */
    struct codec_data_node data_list[DATA_NODE_MAX];
    rt_uint16_t read_index, put_index;

    /* i2c mode */
    struct rt_i2c_bus_device * i2c_device;
};
struct codec_device codec;

static uint16_t r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV2 | BCLK_DIV8;

#if !CODEC_MASTER_MODE
static int codec_sr_new = 0;
#endif

static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* DMA IRQ Channel configuration */
    NVIC_InitStructure.NVIC_IRQChannel = AUDIO_I2S_DMA_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
I2S3S_WS    PA15
I2S3_CK     PB3
I2S3ext_SD  PB4
I2S3_SD     PB5
*/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

    /* Connect pins to I2S peripheral  */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_I2S3ext);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);

    /* I2S3S_WS */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* I2S3_CK, I2S3_SD, I2S3ext_SD */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void DMA_Configuration(rt_uint32_t addr, rt_size_t size)
{
    DMA_InitTypeDef DMA_InitStructure;

    /* Configure the DMA Stream */
    DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);
    DMA_DeInit(AUDIO_I2S_DMA_STREAM);

    /* Set the parameters to be configured */
    DMA_InitStructure.DMA_Channel = AUDIO_I2S_DMA_CHANNEL;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&CODEC_I2S_PORT->DR);
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)addr;      /* This field will be configured in play function */
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = (uint32_t)size;      /* This field will be configured in play function */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(AUDIO_I2S_DMA_STREAM, &DMA_InitStructure);

    /* Enable SPI DMA Tx request */
    SPI_I2S_DMACmd(CODEC_I2S_PORT, SPI_I2S_DMAReq_Tx, ENABLE);

    DMA_ITConfig(AUDIO_I2S_DMA_STREAM, DMA_IT_TC, ENABLE);
    DMA_Cmd(AUDIO_I2S_DMA_STREAM, ENABLE);
}

static void I2S_Configuration(uint32_t I2S_AudioFreq)
{
    I2S_InitTypeDef I2S_InitStructure;

    /* I2S peripheral configuration */
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;

    /* I2S2 configuration */
#if CODEC_MASTER_MODE
    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
#else
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
#endif
    I2S_Init(CODEC_I2S_PORT, &I2S_InitStructure);
}

static void codec_send(rt_uint16_t s_data)
{
    struct rt_i2c_msg msg;
    rt_uint8_t send_buffer[2];

    RT_ASSERT(codec.i2c_device != RT_NULL);

    send_buffer[0] = (rt_uint8_t)(s_data>>8);
    send_buffer[1] = (rt_uint8_t)(s_data);

    msg.addr = 0x1A;
    msg.flags = RT_I2C_WR;
    msg.len = 2;
    msg.buf = send_buffer;

    rt_i2c_transfer(codec.i2c_device, &msg, 1);
}

static rt_err_t codec_init(rt_device_t dev)
{
    codec_send(REG_SOFTWARE_RESET);

    // 1.5x boost power up sequence.
    // Mute all outputs.
    codec_send(REG_LOUT1_VOL | LOUT1MUTE);
    codec_send(REG_ROUT1_VOL | ROUT1MUTE);
    codec_send(REG_LOUT2_VOL | LOUT2MUTE);
    codec_send(REG_ROUT2_VOL | ROUT2MUTE);
    // Enable unused output chosen from L/ROUT2, OUT3 or OUT4.
    codec_send(REG_POWER_MANAGEMENT3 | OUT4EN);
    // Set BUFDCOPEN=1 and BUFIOEN=1 in register R1
    codec_send(REG_POWER_MANAGEMENT1 | BUFDCOPEN | BUFIOEN);
    // Set SPKBOOST=1 in register R49.
    codec_send(REG_OUTPUT | SPKBOOST);
    // Set VMIDSEL[1:0] to required value in register R1.
    codec_send(REG_POWER_MANAGEMENT1 | BUFDCOPEN | BUFIOEN | VMIDSEL_75K);
    // Set L/RMIXEN=1 and DACENL/R=1 in register R3.
    codec_send(REG_POWER_MANAGEMENT3 | LMIXEN | RMIXEN | DACENL | DACENR);
    // Set BIASEN=1 in register R1.
    codec_send(REG_POWER_MANAGEMENT1 | BUFDCOPEN | BUFIOEN | VMIDSEL_75K);
    //codec_send(REG_POWER_MANAGEMENT1 | BUFDCOPEN | BUFIOEN | VMIDSEL_75K | BIASEN);
    // Set L/ROUT2EN=1 in register R3.
    codec_send(REG_POWER_MANAGEMENT3 | LMIXEN | RMIXEN | DACENL | DACENR | LOUT2EN | ROUT2EN);
    // Enable other mixers as required.
    // Enable other outputs as required.
    codec_send(REG_POWER_MANAGEMENT2 | LOUT1EN | ROUT1EN | BOOSTENL | BOOSTENR | INPPGAENL | INPPGAENR);

    // Digital inferface setup.
    codec_send(REG_AUDIO_INTERFACE | BCP_NORMAL | LRP_NORMAL | WL_16BITS | FMT_I2S);

    // PLL setup. (MCLK: 12.2896 for 44.1K)
    codec_send(REG_PLL_N  | PLL_N_112896);
    codec_send(REG_PLL_K1 | ((PLL_K_112896>>18) & 0x1F));
    codec_send(REG_PLL_K2 | ((PLL_K_112896>>9) & 0x1FF));
    codec_send(REG_PLL_K3 | ((PLL_K_112896>>0) & 0x1FF));

    codec_send(REG_POWER_MANAGEMENT1 | BUFDCOPEN | BUFIOEN | VMIDSEL_75K | BIASEN | PLLEN);
    codec_send(r06);

    // Enable DAC 128x oversampling.
    codec_send(REG_DAC | DACOSR128);

    // Set LOUT2/ROUT2 in BTL operation.
    codec_send(REG_BEEP | INVROUT2);

    // Set output volume.
    vol(65);

    return RT_EOK;
}

void vol(uint16_t v) // 0~99
{
    v = 63*v/100;
    v = (v & VOL_MASK) << VOL_POS;
    codec_send(REG_LOUT1_VOL | v);
    codec_send(REG_ROUT1_VOL | HPVU | v);
    codec_send(REG_LOUT2_VOL | v);
    codec_send(REG_ROUT2_VOL | SPKVU | v);
}

void eq(codec_eq_args_t args)
{
    switch (args->channel)
    {
    case 1:
        codec_send(REG_EQ1 | ((args->frequency & EQC_MASK) << EQC_POS) | ((args->gain & EQG_MASK) << EQG_POS) | (args->mode_bandwidth ? EQ3DMODE_DAC : EQ3DMODE_ADC));
        break;

    case 2:
        codec_send(REG_EQ2 | ((args->frequency & EQC_MASK) << EQC_POS) | ((args->gain & EQG_MASK) << EQG_POS) | (args->mode_bandwidth ? EQ2BW_WIDE : EQ2BW_NARROW));
        break;

    case 3:
        codec_send(REG_EQ3 | ((args->frequency & EQC_MASK) << EQC_POS) | ((args->gain & EQG_MASK) << EQG_POS) | (args->mode_bandwidth ? EQ3BW_WIDE : EQ3BW_NARROW));
        break;

    case 4:
        codec_send(REG_EQ4 | ((args->frequency & EQC_MASK) << EQC_POS) | ((args->gain & EQG_MASK) << EQG_POS) | (args->mode_bandwidth ? EQ4BW_WIDE : EQ4BW_NARROW));
        break;

    case 5:
        codec_send(REG_EQ5 | ((args->frequency & EQC_MASK) << EQC_POS) | ((args->gain & EQG_MASK) << EQG_POS));
        break;
    }
}

// TODO eq1() ~ eq5() are just for testing. To be removed.
void eq1(uint8_t freq, uint8_t gain, uint8_t mode)
{
    codec_send(REG_EQ1 | ((freq & EQC_MASK) << EQC_POS) | ((gain & EQG_MASK) << EQG_POS) | (mode ? EQ3DMODE_DAC : EQ3DMODE_ADC));
}

void eq2(uint8_t freq, uint8_t gain, uint8_t bw)
{
    codec_send(REG_EQ2 | ((freq & EQC_MASK) << EQC_POS) | ((gain & EQG_MASK) << EQG_POS) | (bw ? EQ2BW_WIDE : EQ2BW_NARROW));
}

void eq3(uint8_t freq, uint8_t gain, uint8_t bw)
{
    codec_send(REG_EQ3 | ((freq & EQC_MASK) << EQC_POS) | ((gain & EQG_MASK) << EQG_POS) | (bw ? EQ3BW_WIDE : EQ3BW_NARROW));
}

void eq4(uint8_t freq, uint8_t gain, uint8_t bw)
{
    codec_send(REG_EQ4 | ((freq & EQC_MASK) << EQC_POS) | ((gain & EQG_MASK) << EQG_POS) | (bw ? EQ4BW_WIDE : EQ4BW_NARROW));
}

void eq5(uint8_t freq, uint8_t gain)
{
    codec_send(REG_EQ2 | ((freq & EQC_MASK) << EQC_POS) | ((gain & EQG_MASK) << EQG_POS));
}

void eq3d(uint8_t depth)
{
    codec_send(REG_3D | ((depth & DEPTH3D_MASK) << DEPTH3D_POS));
}

rt_err_t sample_rate(int sr)
{
    uint16_t r07 = REG_ADDITIONAL;
    uint32_t PLL_N, PLL_K;

    switch (sr)
    {
    case 8000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV12 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_8KHZ;
        break;

    case 11025: /* MCLK : 11.2896 */
        PLL_N = PLL_N_112896;
        PLL_K = PLL_K_112896;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV8 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_12KHZ;
        break;

#if CODEC_MASTER_MODE
    case 12000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV8 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_12KHZ;
        break;
#endif

    case 16000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV6 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_16KHZ;
        break;

    case 22050: /* MCLK : 11.2896 */
        PLL_N = PLL_N_112896;
        PLL_K = PLL_K_112896;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV4 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_24KHZ;
        break;

    case 24000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV4 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_24KHZ;
        break;

    case 32000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV3 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_32KHZ;
        break;

    case 44100: /* MCLK : 11.2896 */
        PLL_N = PLL_N_112896;
        PLL_K = PLL_K_112896;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV2 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_48KHZ;
        break;

    case 48000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV2 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_48KHZ;
        break;

    /* Warning: in WM8978 datasheet, sample rate 96K doesn't support. */
    case 96000: /* MCLK : 12.288 */
        PLL_N = PLL_N_122880;
        PLL_K = PLL_K_122880;
        r06 = REG_CLOCK_GEN | CLKSEL_PLL | MCLK_DIV1 | BCLK_DIV8 | (r06 & MS);
        r07 |= SR_48KHZ;
        break;

    default:
        return RT_ERROR;
    }
    // PLL setup. (MCLK: 12.2896 for 44.1K)
    codec_send(REG_PLL_N  | PLL_N);
    codec_send(REG_PLL_K1 | ((PLL_K>>18) & 0x1F));
    codec_send(REG_PLL_K2 | ((PLL_K>>9) & 0x1FF));
    codec_send(REG_PLL_K3 | ((PLL_K>>0) & 0x1FF));

    codec_send(r06);
    codec_send(r07);

#if !CODEC_MASTER_MODE
    codec_sr_new = sr;
#endif

    return RT_EOK;
}

/* Exported functions */
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(vol, Set volume);
FINSH_FUNCTION_EXPORT(eq1, Set EQ1(Cut-off, Gain, Mode));
FINSH_FUNCTION_EXPORT(eq2, Set EQ2(Center, Gain, Bandwidth));
FINSH_FUNCTION_EXPORT(eq3, Set EQ3(Center, Gain, Bandwidth));
FINSH_FUNCTION_EXPORT(eq4, Set EQ4(Center, Gain, Bandwidth));
FINSH_FUNCTION_EXPORT(eq5, Set EQ5(Cut-off, Gain));
FINSH_FUNCTION_EXPORT(eq3d, Set 3D(Depth));
FINSH_FUNCTION_EXPORT(sample_rate, Set sample rate);
#endif

static rt_err_t codec_open(rt_device_t dev, rt_uint16_t oflag)
{
#if !CODEC_MASTER_MODE
    /* enable I2S */
    I2S_Cmd(CODEC_I2S_PORT, ENABLE);
#endif

    return RT_EOK;
}

static rt_err_t codec_close(rt_device_t dev)
{
#if CODEC_MASTER_MODE
    if (r06 & MS)
    {
        NVIC_DisableIRQ(AUDIO_I2S_DMA_IRQ);
        DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);
        /* Clear DMA Stream Transfer Complete interrupt pending bit */
        DMA_ClearITPendingBit(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_IT_TC);

        while ((CODEC_I2S_PORT->SR & SPI_I2S_FLAG_TXE) == 0);
        while ((CODEC_I2S_PORT->SR & SPI_I2S_FLAG_BSY) != 0);
        I2S_Cmd(CODEC_I2S_PORT, DISABLE);

        r06 &= ~MS;
        codec_send(r06);

        /* remove all data node */
        if (codec.parent.tx_complete != RT_NULL)
        {
            rt_base_t level = rt_hw_interrupt_disable();

            while (codec.read_index != codec.put_index)
            {
                codec.parent.tx_complete(&codec.parent, codec.data_list[codec.read_index].data_ptr);
                codec.read_index++;
                if (codec.read_index >= DATA_NODE_MAX)
                {
                    codec.read_index = 0;
                }
            }

            rt_hw_interrupt_enable(level);
        }
    }
#endif

    return RT_EOK;
}

static rt_err_t codec_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_err_t result = RT_EOK;

    switch (cmd)
    {
    case CODEC_CMD_RESET:
        result = codec_init(dev);
        break;

    case CODEC_CMD_VOLUME:
        vol(*((uint16_t*) args));
        break;

    case CODEC_CMD_SAMPLERATE:
        result = sample_rate(*((int*) args));
        break;

    case CODEC_CMD_EQ:
        eq((codec_eq_args_t) args);
        break;

    case CODEC_CMD_3D:
        eq3d(*((uint8_t*) args));
        break;

    default:
        result = RT_ERROR;
    }
    return result;
}

static rt_size_t codec_write(rt_device_t dev, rt_off_t pos,
                             const void* buffer, rt_size_t size)
{
    struct codec_device* device;
    struct codec_data_node* node;
    rt_uint32_t level;
    rt_uint16_t next_index;

    device = (struct codec_device*) dev;
    RT_ASSERT(device != RT_NULL);

    next_index = device->put_index + 1;
    if (next_index >= DATA_NODE_MAX)
        next_index = 0;

    /* check data_list full */
    if (next_index == device->read_index)
    {
        rt_set_errno(-RT_EFULL);
        return 0;
    }

    level = rt_hw_interrupt_disable();
    node = &device->data_list[device->put_index];
    device->put_index = next_index;

    /* set node attribute */
    node->data_ptr = (rt_uint16_t*) buffer;
    node->data_size = size >> 1; /* size is byte unit, convert to half word unit */

    next_index = device->read_index + 1;
    if (next_index >= DATA_NODE_MAX)
        next_index = 0;

    /* check data list whether is empty */
    if (next_index == device->put_index)
    {
#if CODEC_MASTER_MODE
        codec_send(r06 & ~MS);
        I2S_Cmd(CODEC_I2S_PORT, DISABLE);
#endif

        NVIC_EnableIRQ(AUDIO_I2S_DMA_IRQ);
        DMA_Configuration((rt_uint32_t) node->data_ptr, node->data_size);

#if CODEC_MASTER_MODE
        if ((r06 & MS) == 0)
        {
            I2S_Cmd(CODEC_I2S_PORT, ENABLE);
            r06 |= MS;
            codec_send(r06);
        }
#endif
    }
    rt_hw_interrupt_enable(level);

    return size;
}

rt_err_t codec_hw_init(const char * i2c_bus_device_name)
{
    struct rt_i2c_bus_device * i2c_device;

    i2c_device = rt_i2c_bus_device_find(i2c_bus_device_name);
    if(i2c_device == RT_NULL)
    {
        rt_kprintf("i2c bus device %s not found!\r\n", i2c_bus_device_name);
        return -RT_ENOSYS;
    }
    codec.i2c_device = i2c_device;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA |
                           RCC_AHB1Periph_GPIOB |
                           RCC_AHB1Periph_GPIOC, ENABLE);
    /* Enable the CODEC_I2S peripheral clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    RCC_PLLI2SConfig(192, 3);
    RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);

    NVIC_Configuration();
    GPIO_Configuration();
    I2S_Configuration(I2S_AudioFreq_96k);

    codec.parent.type = RT_Device_Class_Sound;
    codec.parent.rx_indicate = RT_NULL;
    codec.parent.tx_complete = RT_NULL;
    codec.parent.user_data   = RT_NULL;

    codec.parent.control = codec_control;
    codec.parent.init    = codec_init;
    codec.parent.open    = codec_open;
    codec.parent.close   = codec_close;
    codec.parent.read    = RT_NULL;
    codec.parent.write   = codec_write;

    /* set read_index and put index to 0 */
    codec.read_index = 0;
    codec.put_index = 0;

    /* register the device */
    return rt_device_register(&codec.parent, "snd", RT_DEVICE_FLAG_WRONLY | RT_DEVICE_FLAG_DMA_TX);
}

static void codec_dma_isr(void)
{
    /* switch to next buffer */
    rt_uint16_t next_index;
    void* data_ptr;

    /* enter interrupt */
    rt_interrupt_enter();

    next_index = codec.read_index + 1;
    if (next_index >= DATA_NODE_MAX)
        next_index = 0;

    /* save current data pointer */
    data_ptr = codec.data_list[codec.read_index].data_ptr;

#if !CODEC_MASTER_MODE
    if (codec_sr_new)
    {
        I2S_Configuration(codec_sr_new);
        I2S_Cmd(CODEC_I2S_PORT, ENABLE);
        codec_sr_new = 0;
    }
#endif

    codec.read_index = next_index;
    if (next_index != codec.put_index)
    {
        /* enable next dma request */
        DMA_Configuration((rt_uint32_t)codec.data_list[codec.read_index].data_ptr,
                          codec.data_list[codec.read_index].data_size);

#if CODEC_MASTER_MODE
        if ((r06 & MS) == 0)
        {
//            CODEC_I2S_PORT->I2SCFGR |= SPI_I2SCFGR_I2SE;
            r06 |= MS;
//            codec_send(r06);
        }
#endif
    }
    else /* codec tx done. */
    {
#if CODEC_MASTER_MODE
        if (r06 & MS)
        {
            DMA_Cmd(AUDIO_I2S_DMA_STREAM, DISABLE);

            while ((CODEC_I2S_PORT->SR & SPI_I2S_FLAG_TXE) == 0);
            while ((CODEC_I2S_PORT->SR & SPI_I2S_FLAG_BSY) != 0);
            I2S_Cmd(CODEC_I2S_PORT, DISABLE);

            r06 &= ~MS;
//            codec_send(r06);
        }
#endif

        rt_kprintf("*\n");
    } /* codec tx done. */

    /* notify transmitted complete. */
    if (codec.parent.tx_complete != RT_NULL)
    {
        codec.parent.tx_complete(&codec.parent, data_ptr);
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

void DMA1_Stream7_IRQHandler(void)
{
    /* Test on DMA Stream Transfer Complete interrupt */
    if(DMA_GetITStatus(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_IT_TC))
    {
        /* Clear DMA Stream Transfer Complete interrupt pending bit */
        DMA_ClearITPendingBit(AUDIO_I2S_DMA_STREAM, AUDIO_I2S_DMA_IT_TC);

        /* do something */
        codec_dma_isr();
    }
}
