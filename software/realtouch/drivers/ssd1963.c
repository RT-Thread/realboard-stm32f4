/**********************************************************************************************************
2013-9-22
by jiezhi320   ssd1963 driver
**********************************************************************************************************/
#include "ssd1963.h"
#include "stm32f4xx.h"
#include "board.h"

#if LCD_VERSION==2

#include "rtthread.h"
#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#endif


//输出重定向
#define printf               rt_kprintf //使用rt_kprintf来输出
//#define printf(...)  

/* LCD is connected to the FSMC_Bank1_NOR/SRAM4 and NE4 is used as ship select signal */
/* RS <==> A0 */
#define LCD_REG              (*((volatile unsigned short *) 0x6C000000)) /* RS = 0 */
#define LCD_RAM              (*((volatile unsigned short *) (0x6C000000 | 0x02 << 19))) /* RS = 1 */
#define NEX_SELECT            FSMC_Bank1_NORSRAM4


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

#define  HDP 	(LCD_WIDTH-1)//799
#define  HT 	928
#define  HPS 	46
#define  LPS 	15
#define  HPW 	48

#define  VDP 	(LCD_HEIGHT-1)//479
#define  VT 	525
#define  VPS 	16
#define  FPS 	8
#define  VPW 	16


//static rt_uint32_t deviceid;     //设置一个静态变量用来保存LCD的ID 
struct rt_device _lcd_device;	 //设备框架结构体  RTGRAPHIC_PIXEL_FORMAT_RGB565

static void delayms(rt_uint32_t cnt)
{
    volatile rt_uint32_t dl;
    while(cnt--)
    {
        for(dl=0; dl<2000; dl++);
    }
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
    Timing_read.FSMC_AddressHoldTime = 1;//2;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_read.FSMC_DataSetupTime = 9;//19;     /* [15:8] F2/F4 0~255 HCLK */
    /* [19:16] Time between NEx high to NEx low (BUSTURN HCLK) */
    Timing_read.FSMC_BusTurnAroundDuration = 1;
    Timing_read.FSMC_CLKDivision = 0; /* [24:20] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_DataLatency = 0; /* [27:25] keep 0x00 in SRAM mode  */
    Timing_read.FSMC_AccessMode = FSMC_AccessMode_A;

    /*--------------------- write timings configuration ---------------------*/
    Timing_write.FSMC_AddressSetupTime = 5;  /* [3:0] F2/F4 1~15 HCLK */
    Timing_write.FSMC_AddressHoldTime = 1;//2;   /* [7:4] keep 0x00 in SRAM mode */
    Timing_write.FSMC_DataSetupTime = 9;//19;     /* [15:8] F2/F4 0~255 HCLK */
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
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
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

static void lcd_port_init(void)
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
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*NWAIT Pin PD6  nwait pin is not use
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    */
	
    // back light control PE5
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;//GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	
    LCD_FSMCConfig();
}

lcd_inline void write_reg(rt_uint16_t cmd)
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

/*
//返回LCD的ID
rt_uint32_t lcd_getdeviceid(void)
{
    return deviceid;
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

*/
lcd_inline void _set_window(u16 x1, u16 y1, u16 x2, u16 y2)  
{  
	write_reg(0x002A);  
    write_data(x1>>8);        
    write_data(x1&0x00ff);  
    write_data(x2>>8);  
    write_data(x2&0x00ff);  
    write_reg(0x002b);  
    write_data(y1>>8);  
    write_data(y1&0x00ff);  
    write_data(y2>>8);  
    write_data(y2&0x00ff);  
}  
   
lcd_inline void _set_cursor(u16 x,u16 y)  
{  
	_set_window(x, y, LCD_WIDTH, LCD_HEIGHT);  
}  

#if 1
static void lcd_clear(rt_uint16_t Color)
{
    rt_uint32_t index=0;

    _set_cursor(0,0);
    write_reg(0x2c);          /* Prepare to write GRAM */
    for (index=0; index<(LCD_WIDTH*LCD_HEIGHT); index++)
    {
        write_data(Color);
    }
	
}
#endif 


static void lcd_data_bus_test(void)
{
    unsigned short temp1;
    unsigned short temp2;

    _set_cursor(0,0);
    write_reg(0x2c);          /* Prepare to write GRAM */
    write_data(0x5566);

    _set_cursor(1,0);
    write_reg(0x2c);          /* Prepare to write GRAM */
    write_data(0xAAbb);

    /* read */
    _set_cursor(0,0);
	
	write_reg(0x2e);  
	temp1 = read_data();  	
	temp2 = read_data();  		

    if( (temp1 == 0x5566) && (temp2 == 0xAAbb) )
    {
        printf(" lcd data bus test pass!");
    }
    else
    {
        printf(" lcd data bus test error: %04X %04X",temp1,temp2);
    }
}


void lcd_Initializtion(void)
{
    lcd_port_init();
	delayms(100); 
	GPIO_ResetBits(GPIOE, GPIO_Pin_5);  
	delayms(1000); 	
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);  /* RESET LCD */
    delayms(1000);
    GPIO_SetBits(GPIOC, GPIO_Pin_6);  /* release LCD */	
	delayms(1000);	
	
	write_reg(0x00E2);	//PLL multiplier, set PLL clock to 120M
	write_data(0x0023);	    //N=0x36 for 6.5M, 0x23 for 10M crystal
	write_data(0x0002);
	write_data(0x0004);
	write_reg(0x00E0);  // PLL enable
	write_data(0x0001);
	delayms(5);
	write_reg(0x00E0);
	write_data(0x0003);
	delayms(5);
	write_reg(0x0001);  // software reset
	delayms(50);
	write_reg(0x00E6);	//PLL setting for PCLK, depends on resolution
	write_data(0x0003);
	write_data(0x00ff);
	write_data(0x00ff);

	write_reg(0x00B0);	//LCD SPECIFICATION
	write_data(0x0027);
	write_data(0x0000);
	write_data((HDP>>8)&0X00FF);  //Set HDP
	write_data(HDP&0X00FF);
    write_data((VDP>>8)&0X00FF);  //Set VDP
	write_data(VDP&0X00FF);
    write_data(0x0000);

	write_reg(0x00B4);	//HSYNC
	write_data((HT>>8)&0X00FF);  //Set HT
	write_data(HT&0X00FF);
	write_data((HPS>>8)&0X00FF);  //Set HPS
	write_data(HPS&0X00FF);
	write_data(HPW);			   //Set HPW
	write_data((LPS>>8)&0X00FF);  //Set HPS
	write_data(LPS&0X00FF);
	write_data(0x0000);

	write_reg(0x00B6);	//VSYNC
	write_data((VT>>8)&0X00FF);   //Set VT
	write_data(VT&0X00FF);
	write_data((VPS>>8)&0X00FF);  //Set VPS
	write_data(VPS&0X00FF);
	write_data(VPW);			   //Set VPW
	write_data((FPS>>8)&0X00FF);  //Set FPS
	write_data(FPS&0X00FF);

	write_reg(0x00BA);
	write_data(0x000F);    //GPIO[3:0] out 1

	write_reg(0x00B8);
	write_data(0x0007);    //GPIO3=input, GPIO[2:0]=output
	write_data(0x0001);    //GPIO0 normal

	write_reg(0x0036); //rotation
	write_data(0x0000);

	write_reg(0x00F0); //pixel data interface
	write_data(0x0003);


	delayms(5);

	write_reg(0x0029); //display on

	write_reg(0x00d0); 
	write_data(0x000d);
	delayms(5);
    //数据总线测试,用于测试硬件连接是否正常.
    lcd_data_bus_test();

    //清屏
    lcd_clear( Blue );
	
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
void rt_hw_lcd_set_pixel(const char* c, int x, int y)
{
	
	rt_uint16_t p;
	p =  *(uint16_t *)c;
	
	_set_cursor(x, y);  
	write_reg(0x2c);  
	write_data(p);  
	
}

/* 获取像素点颜色 */
void rt_hw_lcd_get_pixel(char* c, int x, int y)
{
	_set_cursor(x, y);  
	write_reg(0x2e);  
	*(rt_uint16_t*)c = read_data();  
	
}


/* 画水平线 */
void rt_hw_lcd_draw_hline(const char* c, int x1, int x2, int y)
{
	
	rt_uint16_t p;
	
	p = *(uint16_t *)c;

    _set_cursor(x1, y);
    write_reg(0x2c);
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

	_set_window(x, y1, x, y2);  
	write_reg(0x2c);  
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

    _set_cursor(x, y);
    write_reg(0x2c);  
    while (size)
    {
        write_data(*ptr ++);
        size--;
    }
}

/* 下面的东东，就有linux 设备驱动框架的味道了 */
struct rt_device_graphic_ops lcd_ili_ops =
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

		info->width = LCD_WIDTH;
        info->height = LCD_HEIGHT;

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

/* 需直接调用的 用于硬件初始化和注册设备 */
void ssd1963_init(void)
{
    /* register lcd device */
    _lcd_device.type  = RT_Device_Class_Graphic;
    _lcd_device.init  = lcd_init;
    _lcd_device.open  = lcd_open;
    _lcd_device.close = lcd_close;
    _lcd_device.control = lcd_control;
    _lcd_device.read  = RT_NULL;
    _lcd_device.write = RT_NULL;

    _lcd_device.user_data = &lcd_ili_ops;

    lcd_Initializtion();

    /* register graphic device driver */
    rt_device_register(&_lcd_device, "lcd",
                       RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}

/*以下提供finsh接口，用于调屏*/
#ifdef RT_USING_FINSH
#include "finsh.h"
void tft_write_reg(unsigned char reg_addr,unsigned short reg_val)
{
    write_reg(reg_addr);
    write_data(reg_val);
}

FINSH_FUNCTION_EXPORT(tft_write_reg, addr & val ) ;
#endif //RT_USING_FINSH

#endif


