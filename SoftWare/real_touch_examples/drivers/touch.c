#include <stdbool.h>

#include "board.h"
#include "touch.h"

/*
TOUCH INT: PA3
*/
#define IS_TOUCH_UP()     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)

#ifdef SAVE_CALIBRATION
#include "setup.h"
#endif

#include <rtthread.h>
#include <rtdevice.h>
#include <rtgui/event.h>
#include <rtgui/kbddef.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>

/*
7  6 - 4  3      2     1-0
s  A2-A0 MODE SER/DFR PD1-PD0
*/
/* X Y change. */
#define TOUCH_MSR_Y  0xD0   // read X addr:1
#define TOUCH_MSR_X  0x90   // read Y addr:3

struct rtgui_touch_device
{
    struct rt_device parent;

    rt_uint16_t x, y;

    rt_bool_t calibrating;
    rt_touch_calibration_func_t calibration_func;

    rt_uint16_t min_x, max_x;
    rt_uint16_t min_y, max_y;

    struct rt_spi_device * spi_device;
    struct rt_event event;
};
static struct rtgui_touch_device *touch = RT_NULL;

rt_inline void touch_int_cmd(FunctionalState NewState);


#define X_WIDTH 800
#define Y_WIDTH 480

static void rtgui_touch_calculate(void)
{
    if (touch != RT_NULL)
    {
        /* read touch */
        {
            unsigned int i;
            rt_uint16_t tmpx[10];
            rt_uint16_t tmpy[10];
            uint8_t send_buffer[1];
            uint8_t recv_buffer[2];

            for(i=0; i<10; i++)
            {
                send_buffer[0] = TOUCH_MSR_X;
                rt_spi_send_then_recv(touch->spi_device, send_buffer, 1, recv_buffer, 2);
                tmpx[i]  = (recv_buffer[0] & 0x7F) << 4;
                tmpx[i] |= (recv_buffer[1] >> 4) & 0x0F;

                send_buffer[0] = TOUCH_MSR_Y;
                rt_spi_send_then_recv(touch->spi_device, send_buffer, 1, recv_buffer, 2);
                tmpy[i]  = (recv_buffer[0] & 0x7F) << 4;
                tmpy[i] |= (recv_buffer[1] >> 4) & 0x0F;
            }
            send_buffer[0] = 1 << 7;
            rt_spi_send(touch->spi_device, send_buffer, 1);

            /* 去最高值与最低值,再取平均值 */
            {
                rt_uint32_t min_x = 0xFFFF, min_y = 0xFFFF;
                rt_uint32_t max_x = 0,max_y = 0;
                rt_uint32_t total_x = 0;
                rt_uint32_t total_y = 0;
                unsigned int i;

                for(i=0; i<10; i++)
                {
                    if( tmpx[i] < min_x )
                    {
                        min_x = tmpx[i];
                    }
                    if( tmpx[i] > max_x )
                    {
                        max_x = tmpx[i];
                    }
                    total_x += tmpx[i];

                    if( tmpy[i] < min_y )
                    {
                        min_y = tmpy[i];
                    }
                    if( tmpy[i] > max_y )
                    {
                        max_y = tmpy[i];
                    }
                    total_y += tmpy[i];
                }
                total_x = total_x - min_x - max_x;
                total_y = total_y - min_y - max_y;
                touch->x = total_x / 8;
                touch->y = total_y / 8;
                rt_kprintf("touch->x:%d touch->y:%d\r\n", touch->x, touch->y);
            } /* 去最高值与最低值,再取平均值 */
        } /* read touch */

        /* if it's not in calibration status  */
        if (touch->calibrating != RT_TRUE)
        {
            if (touch->max_x > touch->min_x)
            {
                touch->x = (touch->x - touch->min_x) * X_WIDTH/(touch->max_x - touch->min_x);
            }
            else
            {
                touch->x = (touch->min_x - touch->x) * X_WIDTH/(touch->min_x - touch->max_x);
            }

            if (touch->max_y > touch->min_y)
            {
                touch->y = (touch->y - touch->min_y) * Y_WIDTH /(touch->max_y - touch->min_y);
            }
            else
            {
                touch->y = (touch->min_y - touch->y) * Y_WIDTH /(touch->min_y - touch->max_y);
            }
        }
    }
}

static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the EXTI0 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

rt_inline void touch_int_cmd(FunctionalState NewState)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Configure  EXTI  */
    EXTI_InitStructure.EXTI_Line = EXTI_Line3;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;

    EXTI_InitStructure.EXTI_LineCmd = NewState;

    EXTI_ClearITPendingBit(EXTI_Line3);
    EXTI_Init(&EXTI_InitStructure);
}

static void EXTI_Configuration(void)
{
    /* PA3 touch INT */
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
        GPIO_Init(GPIOA, &GPIO_InitStructure);
    }

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);

    /* Configure  EXTI  */
    touch_int_cmd(ENABLE);
}

/* RT-Thread Device Interface */
static rt_err_t rtgui_touch_init (rt_device_t dev)
{
    uint8_t send;
    struct rtgui_touch_device * touch_device = (struct rtgui_touch_device *)dev;

    NVIC_Configuration();
    EXTI_Configuration();

    send = 1 << 7;
    rt_spi_send(touch_device->spi_device, &send, 1); /* enable touch interrupt */

    return RT_EOK;
}

static rt_err_t rtgui_touch_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
    switch (cmd)
    {
    case RT_TOUCH_CALIBRATION:
        touch->calibrating = RT_TRUE;
        touch->calibration_func = (rt_touch_calibration_func_t)args;
        break;

    case RT_TOUCH_NORMAL:
        touch->calibrating = RT_FALSE;
        break;

    case RT_TOUCH_CALIBRATION_DATA:
    {
        struct calibration_data* data;

        data = (struct calibration_data*) args;

        //update
        touch->min_x = data->min_x;
        touch->max_x = data->max_x;
        touch->min_y = data->min_y;
        touch->max_y = data->max_y;

#ifdef SAVE_CALIBRATION
        //save setup
        radio_setup.touch_min_x = touch->min_x;
        radio_setup.touch_max_x = touch->max_x;
        radio_setup.touch_min_y = touch->min_y;
        radio_setup.touch_max_y = touch->max_y;
        save_setup();
#endif
    }
    break;
    }

    return RT_EOK;
}

static void touch_thread_entry(void *parameter)
{
    rt_bool_t touch_down = RT_FALSE;
    rt_uint32_t event_value;
    struct rtgui_event_mouse emouse;
    static struct _touch_previous
    {
        rt_uint32_t x;
        rt_uint32_t y;
    } touch_previous;

    while(1)
    {
        if(rt_event_recv(&touch->event, 1 ,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &event_value) == RT_EOK)
        {
            while(1)
            {
                if (IS_TOUCH_UP())
                {
                    /* touch up */
                    emouse.parent.type = RTGUI_EVENT_MOUSE_BUTTON;
                    emouse.button = (RTGUI_MOUSE_BUTTON_LEFT |RTGUI_MOUSE_BUTTON_UP);

                    /* use old value */
                    emouse.x = touch->x;
                    emouse.y = touch->y;

                    if ((touch->calibrating == RT_TRUE) && (touch->calibration_func != RT_NULL))
                    {
                        /* callback function */
                        touch->calibration_func(emouse.x, emouse.y);
                    }
                    else
                    {
                        rtgui_server_post_event(&emouse.parent, sizeof(struct rtgui_event_mouse));
                    }

                    /* clean */
                    touch_previous.x = touch_previous.y = 0;
                    touch_down = RT_FALSE;

                    touch_int_cmd(ENABLE);
                    break;
                } /* touch up */
                else /* touch down or move */
                {
                    /* calculation */
                    rtgui_touch_calculate();

                    /* send mouse event */
                    emouse.parent.type = RTGUI_EVENT_MOUSE_BUTTON;
                    emouse.parent.sender = RT_NULL;

                    emouse.x = touch->x;
                    emouse.y = touch->y;

                    /* init mouse button */
                    emouse.button = (RTGUI_MOUSE_BUTTON_LEFT |RTGUI_MOUSE_BUTTON_DOWN);

                    /* send event to server */
                    if (touch->calibrating != RT_TRUE)
                    {
#define previous_keep      8
                        /* filter. */
                        if(
                            (touch_previous.x > touch->x + previous_keep)
                            || (touch_previous.x < touch->x - previous_keep)
                            || (touch_previous.y > touch->y + previous_keep)
                            || (touch_previous.y < touch->y - previous_keep)  )
                        {
                            touch_previous.x = touch->x;
                            touch_previous.y = touch->y;
                            rtgui_server_post_event(&emouse.parent, sizeof(struct rtgui_event_mouse));
                            if(touch_down == RT_FALSE)
                            {
                                touch_down = RT_TRUE;
//                                rt_kprintf("touch down: (%d, %d)\n", emouse.x, emouse.y);
                            }
                            else
                            {
//                                rt_kprintf("touch motion: (%d, %d)\n", emouse.x, emouse.y);
                            }
                        }
                    }
                } /* touch down or move */

                rt_thread_delay(RT_TICK_PER_SECOND / 10);
            } /* read touch */
        } /* event recv */
    } /* thread while(1) */
}

void EXTI3_IRQHandler(void)
{
    /* disable interrupt */
    touch_int_cmd(DISABLE);

    rt_event_send(&touch->event, 1);

    EXTI_ClearITPendingBit(EXTI_Line3);
}

rt_err_t rtgui_touch_hw_init(const char * spi_device_name)
{
    struct rt_spi_device * spi_device;
    struct rt_thread * touch_thread;

    spi_device = (struct rt_spi_device *)rt_device_find(spi_device_name);
    if(spi_device == RT_NULL)
    {
        rt_kprintf("spi device %s not found!\r\n", spi_device_name);
        return -RT_ENOSYS;
    }

    /* config spi */
    {
        struct rt_spi_configuration cfg;
        cfg.data_width = 8;
        cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible Modes 0 */
        cfg.max_hz = 1 * 1000 * 1000; /* 1M */
        rt_spi_configure(spi_device, &cfg);
    }

    touch = (struct rtgui_touch_device*)rt_malloc (sizeof(struct rtgui_touch_device));
    if (touch == RT_NULL) return RT_ENOMEM; /* no memory yet */

    /* clear device structure */
    rt_memset(&(touch->parent), 0, sizeof(struct rt_device));

    rt_event_init(&touch->event, "touch", RT_IPC_FLAG_FIFO);

    touch->spi_device = spi_device;
    touch->calibrating = false;

//    touch->min_x = radio_setup.touch_min_x;
//    touch->max_x = radio_setup.touch_max_x;
//    touch->min_y = radio_setup.touch_min_y;
//    touch->max_y = radio_setup.touch_max_y;
    touch->min_x = 0;
    touch->max_x = 0;
    touch->min_y = 0;
    touch->max_y = 0;

    /* init device structure */
    touch->parent.type = RT_Device_Class_Unknown;
    touch->parent.init = rtgui_touch_init;
    touch->parent.control = rtgui_touch_control;
    touch->parent.user_data = RT_NULL;

    /* register touch device to RT-Thread */
    rt_device_register(&(touch->parent), "touch", RT_DEVICE_FLAG_RDWR);

    touch_thread = rt_thread_create("touch",
                                    touch_thread_entry, RT_NULL,
                                    512, RTGUI_SVR_THREAD_PRIORITY-1, 1);
    if (touch_thread != RT_NULL) rt_thread_startup(touch_thread);

    return RT_EOK;
}

#ifdef RT_USING_FINSH
#include <finsh.h>

void touch_t( rt_uint16_t x , rt_uint16_t y )
{
    struct rtgui_event_mouse emouse ;
    emouse.parent.type = RTGUI_EVENT_MOUSE_BUTTON;
    emouse.parent.sender = RT_NULL;

    emouse.x = x ;
    emouse.y = y ;
    /* init mouse button */
    emouse.button = (RTGUI_MOUSE_BUTTON_LEFT |RTGUI_MOUSE_BUTTON_DOWN );
    rtgui_server_post_event(&emouse.parent, sizeof(struct rtgui_event_mouse));

    rt_thread_delay(2) ;
    emouse.button = (RTGUI_MOUSE_BUTTON_LEFT |RTGUI_MOUSE_BUTTON_UP );
    rtgui_server_post_event(&emouse.parent, sizeof(struct rtgui_event_mouse));
}

FINSH_FUNCTION_EXPORT(touch_t, x & y ) ;
#endif
