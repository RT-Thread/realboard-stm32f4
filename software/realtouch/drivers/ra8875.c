#include "rtthread.h"
#include "ra8875.h"

/********* control ***********/
#include "board.h"

/* redirect. */
#define printf               rt_kprintf     /* use rt_kprintf. */
//#define printf(...)                       /* none. */

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A19 */
#define LCD_DATA              (*((volatile unsigned short *) 0x6C000000)) /* RS = 0 */
#define LCD_CMD               (*((volatile unsigned short *) (0x6C000000 | 0x02 << 19))) /* RS = 1 */

#define LCD_CmdWrite(cmd)     LCD_CMD = cmd
#define LCD_DataWrite(data)   LCD_DATA = data
#define Delay1ms(tick)        rt_thread_delay(tick)

static struct rt_device _lcd_device;

/* LCD_busy --> PD6 --> FSMC_NWAIT */
rt_inline void _wait_bus_ready(void)
{
    while(!(GPIOD->IDR & GPIO_Pin_6)); // 0-busy 1-ready
}

rt_inline void _wait_lcd_ready(void)
{
    uint16_t status;
    do
    {
        _wait_bus_ready();
        status = LCD_CMD;
    }
    while(status & (1<<7)); // [7] 0-ready 1- busy
}

rt_inline uint16_t LCD_DataRead(void)
{
    _wait_bus_ready();
    return LCD_DATA;
}

static void _set_gpio_od(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PG12 NE4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;

    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void _set_gpio_pp(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* PG12 NE4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOG, &GPIO_InitStructure);
}

static void LCD_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;

    FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);

    /*--------------------- read timings configuration ---------------------*/
    Timing_read.FSMC_AddressSetupTime = 3;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_read.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_read.FSMC_DataSetupTime = 4;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_read.FSMC_BusTurnAroundDuration = 1;
    Timing_read.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;

    /*--------------------- write timings configuration ---------------------*/
    Timing_write.FSMC_AddressSetupTime = 2;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_write.FSMC_AddressHoldTime = 0;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_write.FSMC_DataSetupTime = 3;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_write.FSMC_BusTurnAroundDuration = 1;
    Timing_write.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_write.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_write.FSMC_AccessMode = FSMC_AccessMode_A;


    /* Color LCD configuration ------------------------------------
       LCD configured as follow:
          - Data/Address MUX = Disable
          - Memory Type = SRAM
          - Data Width = 16bit
          - Write Operation = Enable
          - Extended Mode = Enable
          - Asynchronous Wait = Disable */
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

#define P800x480

static void RA8875_PLL_ini(void)
{
#ifdef P800x480
    LCD_CmdWrite(0x88);
    LCD_DataWrite(0x0c);
    Delay1ms(1);
    LCD_CmdWrite(0x89);
    LCD_DataWrite(0x02);
    Delay1ms(1);
#endif
}

static void LCD_Initial(void)
{
    RA8875_PLL_ini();
    LCD_CmdWrite(0x10);  //SYSR   bit[4:3]=00 256 color  bit[2:1]=  00 8bit MPU interface
    LCD_DataWrite(0x0f);   /* [3:2]-256/65K [1:0]-8/18bit */

#ifdef P800x480
//AT070TN92  setting
//==============	Display Window	800x480 ==================
    LCD_CmdWrite(0x04);  //PCLK inverse
    LCD_DataWrite(0x81);
    Delay1ms(1);

    //Horizontal set
    LCD_CmdWrite(0x14); //HDWR//Horizontal Display Width Setting Bit[6:0]
    LCD_DataWrite(0x63);//Horizontal display width(pixels) = (HDWR + 1)*8
    LCD_CmdWrite(0x15);//Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
    LCD_DataWrite(0x03);//Horizontal Non-Display Period Fine Tuning(HNDFT) [3:0]
    LCD_CmdWrite(0x16); //HNDR//Horizontal Non-Display Period Bit[4:0]
    LCD_DataWrite(0x03);//Horizontal Non-Display Period (pixels) = (HNDR + 1)*8
    LCD_CmdWrite(0x17); //HSTR//HSYNC Start Position[4:0]
    LCD_DataWrite(0x02);//HSYNC Start Position(PCLK) = (HSTR + 1)*8
    LCD_CmdWrite(0x18); //HPWR//HSYNC Polarity ,The period width of HSYNC.
    LCD_DataWrite(0x00);//HSYNC Width [4:0]   HSYNC Pulse width(PCLK) = (HPWR + 1)*8

    //Vertical set
    LCD_CmdWrite(0x19); //VDHR0 //Vertical Display Height Bit [7:0]
    LCD_DataWrite(0xdf);//Vertical pixels = VDHR + 1
    LCD_CmdWrite(0x1a); //VDHR1 //Vertical Display Height Bit [8]
    LCD_DataWrite(0x01);//Vertical pixels = VDHR + 1
    LCD_CmdWrite(0x1b); //VNDR0 //Vertical Non-Display Period Bit [7:0]
    LCD_DataWrite(0x14);//Vertical Non-Display area = (VNDR + 1)
    LCD_CmdWrite(0x1c); //VNDR1 //Vertical Non-Display Period Bit [8]
    LCD_DataWrite(0x00);//Vertical Non-Display area = (VNDR + 1)
    LCD_CmdWrite(0x1d); //VSTR0 //VSYNC Start Position[7:0]
    LCD_DataWrite(0x06);//VSYNC Start Position(PCLK) = (VSTR + 1)
    LCD_CmdWrite(0x1e); //VSTR1 //VSYNC Start Position[8]
    LCD_DataWrite(0x00);//VSYNC Start Position(PCLK) = (VSTR + 1)
    LCD_CmdWrite(0x1f); //VPWR //VSYNC Polarity ,VSYNC Pulse Width[6:0]
    LCD_DataWrite(0x01);//VSYNC Pulse Width(PCLK) = (VPWR + 1)

    //Active window  set
    //setting active window X
    LCD_CmdWrite(0x30); //Horizontal Start Point 0 of Active Window (HSAW0)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [7:0]
    LCD_CmdWrite(0x31); //Horizontal Start Point 1 of Active Window (HSAW1)
    LCD_DataWrite(0x00); //Horizontal Start Point of Active Window [9:8]
    LCD_CmdWrite(0x34); //Horizontal End Point 0 of Active Window (HEAW0)
    LCD_DataWrite(0x1F); //Horizontal End Point of Active Window [7:0]
    LCD_CmdWrite(0x35); //Horizontal End Point 1 of Active Window (HEAW1)
    LCD_DataWrite(0x03); //Horizontal End Point of Active Window [9:8]

    //setting active window Y
    LCD_CmdWrite(0x32); //Vertical Start Point 0 of Active Window (VSAW0)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [7:0]
    LCD_CmdWrite(0x33); //Vertical Start Point 1 of Active Window (VSAW1)
    LCD_DataWrite(0x00); //Vertical Start Point of Active Window [8]
    LCD_CmdWrite(0x36); //Vertical End Point of Active Window 0 (VEAW0)
    LCD_DataWrite(0xdf); //Vertical End Point of Active Window [7:0]
    LCD_CmdWrite(0x37); //Vertical End Point of Active Window 1 (VEAW1)
    LCD_DataWrite(0x01); //Vertical End Point of Active Window [8]

#endif
}

//--------------------------------------------//
//REG[46h]~REG[49h]
static void _set_write_cursor(uint32_t X, uint32_t Y)
{
    LCD_CmdWrite(CURH1);
    LCD_DataWrite(X>>8);
    LCD_CmdWrite(CURH0);
    LCD_DataWrite(X);

    LCD_CmdWrite(CURV1);
    LCD_DataWrite(Y>>8);
    LCD_CmdWrite(CURV0);
    LCD_DataWrite(Y);
}

static void _set_read_cursor(uint32_t X, uint32_t Y)
{
    LCD_CmdWrite(RCURH1);
    LCD_DataWrite(X>>8);
    LCD_CmdWrite(RCURH0);
    LCD_DataWrite(X);

    LCD_CmdWrite(RCURV1);
    LCD_DataWrite(Y>>8);
    LCD_CmdWrite(RCURV0);
    LCD_DataWrite(Y);
}

/** \brief LCD brightness PWM setting.
 *
 * \param pwm_duty_cycle int LED pwm duty cycle, 0~100.
 * \return void
 *
 */
static void pwm_setting(int pwm_duty_cycle)
{
    uint32_t value;

    value  = (1 << 7); /* enable PWM. */
    value |= (0 << 6); /* ouput LOW when PWM STOP. */
    value |= (0 << 4); /* selet PWM1 function. */
    value |= 8;        /* 8: PWM clk = SYS_CLK/256. */
    LCD_CmdWrite(0x8A);
    LCD_DataWrite(value);


    value = (pwm_duty_cycle * 256) / 100;
    if(value > 0xFF)
    {
        value = 0xFF;
    }
    LCD_CmdWrite(0x8B);
    LCD_DataWrite(value);
}

static rt_err_t lcd_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t lcd_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t lcd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info;

        info = (struct rt_device_graphic_info*) args;
        RT_ASSERT(info != RT_NULL);

        info->bits_per_pixel = 16;
        info->pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB565P;
        info->framebuffer = RT_NULL;
        info->width = 800;
        info->height = 480;
    }
    break;

    case RTGRAPHIC_CTRL_RECT_UPDATE:
        /* nothong to be done */
        break;

    default:
        break;
    }

    return RT_EOK;
}

static void ra8875_lcd_set_pixel(const char* pixel, int x, int y)
{
    _set_write_cursor(x, y);
    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write
    LCD_DataWrite(*(uint16_t *)pixel);  //write red data
}

static void ra8875_lcd_get_pixel(char* pixel, int x, int y)
{
    _set_read_cursor(x, y);
    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    _set_gpio_od();

    *(rt_uint16_t*)pixel = LCD_DataRead(); /* dummy read */
    *(rt_uint16_t*)pixel = LCD_DataRead();

    _set_gpio_pp();
}

static void ra8875_lcd_draw_hline(const char* pixel, int x1, int x2, int y)
{
    LCD_CmdWrite(0x40);
    LCD_DataWrite(0x00);

    _set_write_cursor(x1,y);

    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    for(; x1<x2; x1++)
    {
        LCD_DataWrite(*(uint16_t *)pixel);  //write red data
    }
}

static void ra8875_lcd_draw_vline(const char* pixel, int x, int y1, int y2)
{
    LCD_CmdWrite(0x40);
    LCD_DataWrite(0x00 | 1<<3);

    _set_write_cursor(x,y1);

    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    for(; y1<y2; y1++)
    {
        LCD_DataWrite(*(uint16_t *)pixel);  //write red data
    }
}

static void ra8875_lcd_blit_line(const char* pixels, int x, int y, rt_size_t size)
{
    rt_uint16_t *ptr;

    LCD_CmdWrite(0x40);
    LCD_DataWrite(0x00);

    _set_write_cursor(x,y);

    ptr = (rt_uint16_t*)pixels;

    LCD_CmdWrite(MRWC);//set CMD02 to  prepare data write

    while(size--)
    {
        LCD_DataWrite(*ptr++);  //write red data
    }
}

static struct rt_device_graphic_ops ra8875_ops =
{
    ra8875_lcd_set_pixel,
    ra8875_lcd_get_pixel,
    ra8875_lcd_draw_hline,
    ra8875_lcd_draw_vline,
    ra8875_lcd_blit_line
};

/*RA8875 reset Pin PC6*/
/*RA8875 INT PE5*/
static void _lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /*RA8875 reset Pin PC6*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*RA8875 INT PE5*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void ra8875_init(void)
{
    _lcd_gpio_init();

    GPIO_ResetBits(GPIOC, GPIO_Pin_6);  /* RESET LCD */
    LCD_FSMCConfig();
    rt_thread_delay(20);
    GPIO_SetBits(GPIOC, GPIO_Pin_6);  /* release LCD */
    rt_thread_delay(20);

    /* register rw test */
    {
        uint8_t tmp1, tmp2;

        LCD_CmdWrite(0x23);
        LCD_DataWrite(0x55);
        LCD_CmdWrite(0x23);
        _wait_bus_ready();
        tmp1 = LCD_DATA;

        LCD_CmdWrite(0x23);
        LCD_DataWrite(0xAA);
        LCD_CmdWrite(0x23);
        _wait_bus_ready();
        tmp2 = LCD_DATA;

        if ((tmp1 == 0x55) && (tmp2 == 0xAA))
        {
            //rt_kprintf("[OK] LCD register rw test pass!\r\n");
        }
        else
        {
            /* error */
            rt_kprintf("[ERR] LCD register rw test failed! %02X %02X\r\n",
                       tmp1, tmp2);
        }
    } /* register rw test */

    LCD_Initial();

    LCD_CmdWrite(0x01);//Display on
    LCD_DataWrite(0x80);

    /*PWM set*/
    pwm_setting(60);

    /*set RA8875 GPOX pin to 1 - disp panel on*/
//	write_reg(0xC7,0x01);
    LCD_CmdWrite(0xC7);
    LCD_DataWrite(0x01);
    /*set lift right*/
    LCD_CmdWrite(0x20);
    LCD_DataWrite(0x08);

    /* data bus test. */
    {
        uint16_t pixel;
        uint32_t i;

        LCD_CmdWrite(0x40);
        LCD_DataWrite(0x00);

        _set_write_cursor(0, 0);

        LCD_CmdWrite(MRWC); //set CMD02 to  prepare data write
        for(i=0; i<800*480; i++)
        {
            LCD_DataWrite(i);
        }

        LCD_CmdWrite(MRCD);
        LCD_DataWrite(0x00);

        _set_read_cursor(0, 0);

        LCD_CmdWrite(MRWC); /* Memory Read Data. */

        _set_gpio_od();

        pixel = LCD_DataRead(); /* dummy read cycle. */

        for(i=0; i<0x10000; i++)
        {
            pixel = LCD_DataRead();
            if(pixel != i)
            {
                rt_kprintf("[ERR] GRAM data error! %d\r\n", i);
                break;
            }
        }

        if(i == 0x10000)
        {
            //rt_kprintf("[OK] GRAM data test pass!\r\n");
        }
        _set_gpio_pp();
    } /* data bus test. */

    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = &ra8875_ops;

    /* register graphic device driver */
    rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}
