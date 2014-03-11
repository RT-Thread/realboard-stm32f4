/**
 * USB device controller driver for RT-Thread RTOS
 *
 */
#include <rtthread.h>
#include <stm32f4xx.h>
#include <rtdevice.h>
#include "usb_core.h"
#include "usb_dcd.h"
#include "usb_dcd_int.h"
#include "usbd_ioreq.h"
#include "usb_bsp.h"

#ifdef RT_USING_USB_DEVICE

static struct udcd stm32_dcd;
ALIGN(4) static USB_OTG_CORE_HANDLE USB_OTG_Core;

void OTG_FS_IRQHandler(void)
{
    extern uint32_t USBD_OTG_ISR_Handler (USB_OTG_CORE_HANDLE *pdev);

    /* enter interrupt */
    rt_interrupt_enter();

    USBD_OTG_ISR_Handler (&USB_OTG_Core);

    rt_interrupt_leave();
}

/**
* @brief  USBD_DeInit
*         Re-Initialize th device library
* @param  pdev: device instance
* @retval status: status
*/
USBD_Status USBD_DeInit(USB_OTG_CORE_HANDLE *pdev)
{
    /* Software Init */

    return USBD_OK;
}

/**
* @brief  USBD_SetupStage
*         Handle the setup stage
* @param  pdev: device instance
* @retval status
*/
static struct ureqest setup;
static rt_uint8_t USBD_SetupStage(USB_OTG_CORE_HANDLE *pdev)
{
    struct udev_msg msg;

    setup.request_type = *(rt_uint8_t *)(pdev->dev.setup_packet);
    setup.request = *(rt_uint8_t *)(pdev->dev.setup_packet + 1);
    setup.value = uswap_8(pdev->dev.setup_packet + 2);
    setup.index = uswap_8(pdev->dev.setup_packet + 4);
    setup.length = uswap_8(pdev->dev.setup_packet + 6);

    pdev->dev.in_ep[0].ctl_data_len = setup.length;

    msg.type = USB_MSG_SETUP_NOTIFY;
    msg.dcd = &stm32_dcd;
    msg.content.setup_msg.packet = (rt_uint32_t*)&setup;

    rt_usbd_post_event(&msg, sizeof(struct udev_msg));

    return USBD_OK;
}

static rt_uint8_t USBD_Reset(USB_OTG_CORE_HANDLE  *pdev)
{
    rt_kprintf("USBD_Reset\n");

    /* Open EP0 OUT */
    DCD_EP_Open(pdev,
                0x00,
                USB_OTG_MAX_EP0_SIZE,
                EP_TYPE_CTRL);

    /* Open EP0 IN */
    DCD_EP_Open(pdev,
                0x80,
                USB_OTG_MAX_EP0_SIZE,
                EP_TYPE_CTRL);

    /* Upon Reset call usr call back */
    pdev->dev.device_status = USB_OTG_DEFAULT;

    return USBD_OK;
}

static rt_uint8_t USBD_DataOutStage(USB_OTG_CORE_HANDLE *pdev , uint8_t epnum)
{
    USB_OTG_EP *ep;

    if(epnum == 0)
    {
        ep = &pdev->dev.out_ep[0];
        if ( pdev->dev.device_state == USB_OTG_EP0_DATA_OUT)
        {
            if(ep->rem_data_len > ep->maxpacket)
            {
                ep->rem_data_len -=  ep->maxpacket;

                if(pdev->cfg.dma_enable == 1)
                {
                    /* in slave mode this, is handled by the RxSTSQLvl ISR */
                    ep->xfer_buff += ep->maxpacket;
                }
                USBD_CtlContinueRx (pdev,
                                    ep->xfer_buff,
                                    MIN(ep->rem_data_len ,ep->maxpacket));
            }
            else
            {
                USBD_CtlSendStatus(pdev);
                rt_completion_done(&stm32_dcd.completion);
            }
        }
    }
    else
    {
        rt_uint16_t size;
        struct udev_msg msg;

        size = ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count;
        msg.type = USB_MSG_DATA_NOTIFY;
        msg.dcd = &stm32_dcd;
        msg.content.ep_msg.ep_addr = epnum;
        msg.content.ep_msg.size = size;

        rt_usbd_post_event(&msg, sizeof(struct udev_msg));
    }

    return USBD_OK;
}

static rt_uint8_t USBD_DataInStage(USB_OTG_CORE_HANDLE *pdev , uint8_t epnum)
{
    USB_OTG_EP *ep;

    ep = &pdev->dev.in_ep[epnum];
    if(epnum == 0)
    {
        if ( pdev->dev.device_state == USB_OTG_EP0_DATA_IN)
        {
            if(ep->rem_data_len > ep->maxpacket)
            {
                ep->rem_data_len -=  ep->maxpacket;
                if(pdev->cfg.dma_enable == 1)
                {
                    /* in slave mode this, is handled by the TxFifoEmpty ISR */
                    ep->xfer_buff += ep->maxpacket;
                }
                USBD_CtlContinueSendData (pdev,
                                          ep->xfer_buff,
                                          ep->rem_data_len);
            }
            else
            {   /* last packet is MPS multiple, so send ZLP packet */
                if((ep->total_data_len % ep->maxpacket == 0) &&
                        (ep->total_data_len >= ep->maxpacket) &&
                        (ep->total_data_len < ep->ctl_data_len ))
                {

                    USBD_CtlContinueSendData(pdev , RT_NULL, 0);
                    ep->ctl_data_len = 0;
                }
                else
                {
                    USBD_CtlReceiveStatus(pdev);
                }
            }
        }
    }
    else
    {           
        struct udev_msg msg;

        msg.type = USB_MSG_DATA_NOTIFY;
        msg.dcd = &stm32_dcd;
        msg.content.ep_msg.ep_addr = epnum | USB_DIR_IN;
        msg.content.ep_msg.size = ep->xfer_len;

        rt_usbd_post_event(&msg, sizeof(struct udev_msg));
    }

    return USBD_OK;
}

static rt_uint8_t USBD_SOF(USB_OTG_CORE_HANDLE  *pdev)
{
    struct udev_msg msg;

    msg.type = USB_MSG_SOF;
    msg.dcd = &stm32_dcd;

    rt_usbd_post_event(&msg, sizeof(struct udev_msg));

    return USBD_OK;
}

static rt_uint8_t USBD_Suspend(USB_OTG_CORE_HANDLE  *pdev)
{
    return USBD_OK;
}

static rt_uint8_t USBD_Resume(USB_OTG_CORE_HANDLE  *pdev)
{
    return USBD_OK;
}

static rt_uint8_t USBD_IsoINIncomplete(USB_OTG_CORE_HANDLE  *pdev)
{
    return USBD_OK;
}

static rt_uint8_t USBD_IsoOUTIncomplete(USB_OTG_CORE_HANDLE  *pdev)
{
    return USBD_OK;
}

USBD_DCD_INT_cb_TypeDef USBD_DCD_INT_cb =
{
    USBD_DataOutStage,
    USBD_DataInStage,
    USBD_SetupStage,
    USBD_SOF,
    USBD_Reset,
    USBD_Suspend,
    USBD_Resume,
    USBD_IsoINIncomplete,
    USBD_IsoOUTIncomplete,
#ifdef VBUS_SENSING_ENABLED
    USBD_DevConnected,
    USBD_DevDisconnected,
#endif
};

USBD_DCD_INT_cb_TypeDef  *USBD_DCD_INT_fops = &USBD_DCD_INT_cb;

static rt_err_t ep_stall(uep_t ep)
{
    if(ep == 0)
    {
        DCD_EP_Stall(&USB_OTG_Core, 0x80);
        DCD_EP_Stall(&USB_OTG_Core, 0);
        USB_OTG_EP0_OutStart(&USB_OTG_Core);
    }
    else
        DCD_EP_Stall(&USB_OTG_Core, ep->ep_desc->bEndpointAddress);

    return RT_EOK;
}

static rt_err_t set_address(rt_uint8_t address)
{
    USB_OTG_Core.dev.device_address = address;
    DCD_EP_SetAddress(&USB_OTG_Core, address);

    return RT_EOK;
}

static rt_err_t clear_feature(rt_uint16_t value, rt_uint16_t index)
{
    if (value == USB_FEATURE_DEV_REMOTE_WAKEUP)
    {
        USB_OTG_Core.dev.DevRemoteWakeup = 0;
    }
    else if (value == USB_FEATURE_ENDPOINT_HALT)
    {
        DCD_EP_ClrStall(&USB_OTG_Core, (rt_uint8_t)(index & 0xFF));
    }
    
    return RT_EOK;
}

static rt_err_t set_feature(rt_uint16_t value, rt_uint16_t index)
{
    if (value == USB_FEATURE_DEV_REMOTE_WAKEUP)
    {
        USB_OTG_Core.dev.DevRemoteWakeup = 1;
    }
    else if (value == USB_FEATURE_ENDPOINT_HALT)
    {
        DCD_EP_Stall(&USB_OTG_Core, (rt_uint8_t)(index & 0xFF));
    }

    return RT_EOK;
}

static rt_uint8_t ep_in_num = 1;
static rt_uint8_t ep_out_num = 1;
static rt_err_t ep_alloc(uep_t ep)
{
    uep_desc_t ep_desc = ep->ep_desc;

    RT_ASSERT(ep != RT_NULL);

    if(ep_desc->bEndpointAddress & USB_DIR_IN)
    {
        if (ep_in_num > 3)
            return -RT_ERROR;
        ep_desc->bEndpointAddress |= ep_in_num++;
    }
    else
    {
        if (ep_out_num > 3)
            return -RT_ERROR;
        ep_desc->bEndpointAddress |= ep_out_num++;
    }

    return RT_EOK;
}

static rt_err_t ep_free(uep_t ep)
{
    return RT_EOK;
}

static rt_err_t ep_run(uep_t ep)
{
    uep_desc_t ep_desc = ep->ep_desc;

    RT_ASSERT(ep != RT_NULL);

    DCD_EP_Open(&USB_OTG_Core, ep_desc->bEndpointAddress,
                ep_desc->wMaxPacketSize, ep_desc->bmAttributes);

    return RT_EOK;
}

static rt_err_t ep_stop(uep_t ep)
{
    RT_ASSERT(ep != RT_NULL);

    DCD_EP_Close(&USB_OTG_Core, ep->ep_desc->bEndpointAddress);

    return RT_EOK;
}

static rt_err_t ep_read(uep_t ep, void *buffer, rt_size_t size)
{
    uep_desc_t ep_desc;

    ep_desc = ep->ep_desc;

    if(ep == 0)
        USBD_CtlPrepareRx(&USB_OTG_Core, buffer, (rt_uint16_t)size);
    else
        DCD_EP_PrepareRx(&USB_OTG_Core, ep_desc->bEndpointAddress, buffer, size);

    return RT_EOK;
}

static rt_size_t ep_write(uep_t ep, void *buffer, rt_size_t size)
{
    rt_uint32_t len;
    uep_desc_t ep_desc;

    ep_desc = ep->ep_desc;

    if(ep == 0)
        len = USBD_CtlSendData(&USB_OTG_Core, buffer, (rt_uint16_t)size);
    else
    {
        len = DCD_EP_Tx(&USB_OTG_Core, ep_desc->bEndpointAddress, buffer, size);
    }

    return len;
}

static rt_err_t send_status(void)
{
    USBD_CtlSendStatus(&USB_OTG_Core);

    return RT_EOK;
}

static struct udcd_ops stm32_dcd_ops =
{
    set_address,
    clear_feature,
    set_feature,
    ep_alloc,
    ep_free,
    ep_stall,
    ep_run,
    ep_stop,
    ep_read,
    ep_write,
    send_status,
};

static rt_err_t stm32_dcd_init(rt_device_t device)
{
    rt_kprintf("stm32_dcd_init\n");

    /* Hardware Init */
    USB_OTG_BSP_Init(&USB_OTG_Core);

    USBD_DeInit(&USB_OTG_Core);

    /* set USB OTG core params */
    DCD_Init(&USB_OTG_Core , USB_OTG_FS_CORE_ID);

    /* Enable Interrupts */
    USB_OTG_BSP_EnableInterrupt(&USB_OTG_Core);

    return RT_EOK;
}

void rt_hw_usbd_init(void)
{
    stm32_dcd.parent.type = RT_Device_Class_USBDevice;
    stm32_dcd.parent.init = stm32_dcd_init;

    stm32_dcd.ops = &stm32_dcd_ops;
    rt_completion_init(&stm32_dcd.completion);

    rt_device_register(&stm32_dcd.parent, "usbd", 0);
}

#endif
