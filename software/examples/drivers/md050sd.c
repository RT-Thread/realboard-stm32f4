#include <rtthread.h>
#include <rtgui/driver.h>
#include "md050sd.h"

/********* control ***********/
#include "board.h"

#if LCD_VERSION==4

//#define _ILI_REVERSE_DIRECTION_

/* redirect. */
#define printf               rt_kprintf     /* use rt_kprintf. */
//#define printf(...)                       /* none. */

/* LCD is connected to the FSMC_Bank1_NOR/SRAM2 and NE2 is used as ship select signal */
/* RS <==> A19 */
#define LCD_RAM              (*((volatile unsigned short *) 0x6C000000)) /* RS = 0 */
#define LCD_REG               (*((volatile unsigned short *) (0x6C000000 | 0x02 << 19))) /* RS = 1 */


/*---------------------- Graphic LCD size definitions ------------------------*/
#define LCD_WIDTH       800                /* Screen Width (in pixels)           */
#define LCD_HEIGHT      480                /* Screen Hight (in pixels)           */

static struct rt_device _lcd_device;

/* 5寸屏 说明
  0,0                 07
  ----------------------------------------------X 800
  |
  |
  |
  |
  | 06
  |
  |
  |
  |
  |
  |
  |
  Y 480
*/

//内联函数定义,用以提高性能
#ifdef __CC_ARM                			 /* ARM Compiler 	*/
#define lcd_inline   				static __inline
#elif defined (__ICCARM__)        		/* for IAR Compiler */
#define lcd_inline 					inline
#elif defined (__GNUC__)        		/* GNU GCC Compiler */
#define lcd_inline 					static __inline
#else
#define lcd_inline                  static
#endif

static void delay(rt_uint32_t cnt)
{
    volatile rt_uint32_t dl;
    while(cnt--)
    {
        for(dl=0; dl<1000; dl++);
    }
}

static void _lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |
                           RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* reset Pin PC6*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*NWAIT Pin PD6*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // back light control PE5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOE, GPIO_Pin_5);
}

/* 总线配置*/
static void LCD_FSMCConfig(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  Timing_read,Timing_write;

    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &Timing_read;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &Timing_write;

    FSMC_NORSRAMStructInit(&FSMC_NORSRAMInitStructure);

    /*--------------------- read timings configuration ---------------------*/
    Timing_read.FSMC_AddressSetupTime = 5;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_read.FSMC_AddressHoldTime = 2;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_read.FSMC_DataSetupTime = 29;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_read.FSMC_BusTurnAroundDuration = 1;
    Timing_read.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;

    /*--------------------- write timings configuration ---------------------*/
    Timing_write.FSMC_AddressSetupTime = 5;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_write.FSMC_AddressHoldTime = 2;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_write.FSMC_DataSetupTime =29  ;   /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_write.FSMC_BusTurnAroundDuration =1;
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
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode =FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}


lcd_inline void write_cmd(rt_uint16_t cmd)
{
    LCD_REG = cmd;
}

lcd_inline rt_uint16_t read_data(void)
{
    return LCD_RAM;
}

lcd_inline void write_data(rt_uint16_t data_code )
{
    LCD_RAM = data_code;
}

lcd_inline void write_reg(rt_uint16_t reg_addr, rt_uint16_t reg_val)
{
    write_cmd(reg_addr);
    write_data(reg_val);
}

lcd_inline unsigned short read_reg(rt_uint16_t reg_addr)
{
    rt_uint16_t val=0;
    write_cmd(reg_addr);
    val = read_data();
    return (val);
}
static void Address_set(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2)
{
#if defined(_ILI_HORIZONTAL_DIRECTION_)
    write_reg(0x02,y1);//y开始
    write_reg(0x03,x1);//x开始

    write_reg(0x06,y2);//y结束
    write_reg(0x07,x2);//x结束
#else

#endif
    write_cmd(0x0f);    //指向数据区
}

static rt_uint16_t BGR2RGB(rt_uint16_t c)
{
    u16  r, g, b, rgb;

    b = (c>>0)  & 0x1f;
    g = (c>>5)  & 0x3f;
    r = (c>>11) & 0x1f;

    rgb =  (b<<11) + (g<<5) + (r<<0);
    return( rgb );
}



/* 读取指定地址的GRAM */
static unsigned short lcd_read_gram(rt_uint32_t x, rt_uint32_t y)
{
    rt_uint16_t temp;

    Address_set(x,y,x,y);

    temp = read_data();

    return temp;
}
#if 1
static void lcd_clear(rt_uint16_t Color)
{
    rt_uint32_t x ,y;

    Address_set(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

    for (y=0; y<LCD_HEIGHT; y++)
    {
        for(x=0; x<LCD_WIDTH; x++)
            write_data(Color);
    }
}
#endif

static void lcd_data_bus_test(void)
{
    unsigned short temp1;
    unsigned short temp2;

    /* wirte */
    Address_set(0,0,1,0);
    write_data(0x5555);
    write_data(0xAAAA);

    /* read */
    Address_set(0,0,1,0);

    temp1 = (lcd_read_gram(0,0));
    temp2 = (lcd_read_gram(1,0));

    if( (temp1 == 0x5555) && (temp2 == 0xAAAA) )
    {
        printf(" data bus test pass!");
    }
    else
    {
        printf(" data bus test error: %04X %04X",temp1,temp2);
    }
}

static void lcd_gram_test(void)
{
    unsigned short temp;
    unsigned int test_x;
    unsigned int test_y;

    printf(" LCD GRAM test....");

    /* write */
    temp=0;

    Address_set(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);

    for(test_y=0; test_y<LCD_HEIGHT*LCD_WIDTH; test_y++)
    {
        write_data(temp);
        temp++;
    }
    /* read */
    temp=0;

    {
        for(test_y=0; test_y<LCD_HEIGHT; test_y++)
        {
            for(test_x=0; test_x<LCD_WIDTH; test_x++)
            {
                if( lcd_read_gram(test_x,test_y) != temp++)
                {
                    printf("  LCD GRAM ERR!!");
                    //while(1);
                    return ;
                }
            }
        }
        printf("  TEST PASS!\r\n");
    }
}


void lcd_Initializtion(void)
{
    _lcd_gpio_init();
    LCD_FSMCConfig();
    delay(30);
    GPIO_ResetBits(GPIOC, GPIO_Pin_6);  /* RESET LCD */
    rt_thread_delay(50);//delay(2000);
    GPIO_SetBits(GPIOC, GPIO_Pin_6);  /* release LCD */
    rt_thread_delay(50);//delay(2000);

    write_reg(0x0001,16);//打开背光

    delay(1000);
    //数据总线测试,用于测试硬件连接是否正常.
    lcd_data_bus_test();

    lcd_clear( Blue2 );
}


void rt_hw_lcd_update(rtgui_rect_t *rect)
{
    /* nothing for none-DMA mode driver */
}

rt_uint8_t * rt_hw_lcd_get_framebuffer(void)
{
    return RT_NULL; /* no framebuffer driver */
}

/*  设置像素点 颜色,X,Y */
void rt_hw_lcd_set_pixel(const char* c, rt_base_t x, rt_base_t y)
{
    rt_uint16_t p;
    p =  *(uint16_t *)c;

    Address_set(x,y,x,y);//设置光标位置
    write_data(p);
}

/* 获取像素点颜色 */
void rt_hw_lcd_get_pixel(char* c, rt_base_t x, rt_base_t y)
{
    rt_uint16_t p;

    p = lcd_read_gram(x,y);

    *(rt_uint16_t*)c = p;
}


/* 画水平线 */
void rt_hw_lcd_draw_hline(const char* c, int x1, int x2, int y)
{
    rt_uint16_t p;

    p = *(uint16_t *)c;

    Address_set(x1,y, x2,y);

    while (x1 < x2)
    {
        write_data(p);
        x1++;
    }
}

/* 垂直线 */
void rt_hw_lcd_draw_vline(const char* c, int x, int y1, int y2)
{
    rt_uint16_t p;

    p = *(uint16_t *)c;


    Address_set(x, y1,x,y2);
    while (y1 < y2)
    {
        write_data(p);
        y1++;
    }
}

/* ?? */
void rt_hw_lcd_draw_blit_line(const char* c, int x, int y, rt_size_t size)
{
    rt_uint16_t *ptr;
    ptr = (rt_uint16_t*)c;

    Address_set(x, y,x+size,y);

    while (size--)
    {
        write_data(*ptr ++);
    }
}

/* 下面的东东，就有linux 设备驱动框架的味道了 */
struct rt_device_graphic_ops md050sd_ops =
{
    rt_hw_lcd_set_pixel,
    rt_hw_lcd_get_pixel,
    rt_hw_lcd_draw_hline,
    rt_hw_lcd_draw_vline,
    rt_hw_lcd_draw_blit_line
};


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
    rt_err_t result = -RT_EIO;

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

        result = RT_EOK;
    }
    break;
    }

    return result;
}


void md050sd_init(void)
{
    lcd_Initializtion();

    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = (void *)&md050sd_ops;

    /* register graphic device driver */
    rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}

#endif
