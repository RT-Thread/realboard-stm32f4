/*
 * File      : core.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2011, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2011-12-12     Yi Qiu      first version
 */

#include <rtthread.h>
#include <drivers/usb_host.h>

static struct uinstance uinst[USB_MAX_DEVICE];

/**
 * This function will allocate an usb device instance from system.
 *
 * @param parent the hub instance to which the new allocated device attached.
 * @param port the hub port.
 *
 * @return the allocate instance on successful, or RT_NULL on failure.
 */
uinst_t rt_usb_alloc_instance(void)
{
    int i;

    /* lock scheduler */
    rt_enter_critical();
    
    for(i=0; i<USB_MAX_DEVICE; i++)
    {
        /* to find an idle instance handle */
        if(uinst[i].status != UINST_STATUS_IDLE) continue;
        
        /* initialize the usb device instance */
        rt_memset(&uinst[i], 0, sizeof(struct uinstance));
        
        uinst[i].status = UINST_STATUS_BUSY;
        uinst[i].index = i + 1;
        uinst[i].address = 0;
        uinst[i].max_packet_size = 0x8;

        /* unlock scheduler */        
        rt_exit_critical();
        return &uinst[i];
    }

    /* unlock scheduler */            
    rt_exit_critical();     

    return RT_NULL;
}

/**
 * This function will attatch an usb device instance to a host controller,
 * and do device enumunation process.
 *
 * @param hcd the host controller driver.
 * @param uinst the usb device instance.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_attatch_instance(uinst_t uinst)
{
    int i = 0;
    rt_err_t ret = RT_EOK;
    struct uconfig_descriptor cfg_desc;
    udev_desc_t dev_desc;
    uintf_desc_t intf_desc;
    ucd_t drv;

    RT_ASSERT(uinst != RT_NULL);
    
    rt_memset(&cfg_desc, 0, sizeof(struct uconfig_descriptor));
    dev_desc = &uinst->dev_desc;

    RT_DEBUG_LOG(RT_DEBUG_USB, ("start enumnation\n"));
    
    /* get device descriptor head */
    ret = rt_usb_get_descriptor(uinst, USB_DESC_TYPE_DEVICE, (void*)dev_desc, 8);
    if(ret != RT_EOK)
    {
        rt_kprintf("get device descriptor head failed\n");
        return ret;
    }
    
    /* set device address */
    ret = rt_usb_set_address(uinst);
    if(ret != RT_EOK)
    {
        rt_kprintf("set device address failed\n");
        return ret;
    }

    /* set device max packet size */
    uinst->max_packet_size = uinst->dev_desc.bMaxPacketSize0;

    RT_DEBUG_LOG(RT_DEBUG_USB, ("get device descriptor length %d\n",
                                dev_desc->bLength));
    
    /* get full device descriptor again */
    ret = rt_usb_get_descriptor
        (uinst, USB_DESC_TYPE_DEVICE, (void*)dev_desc, dev_desc->bLength);
    if(ret != RT_EOK)
    {
        rt_kprintf("get full device descriptor failed\n");
        return ret;
    }

    RT_DEBUG_LOG(RT_DEBUG_USB, ("Vendor ID 0x%x\n", dev_desc->idVendor));
    RT_DEBUG_LOG(RT_DEBUG_USB, ("Product ID 0x%x\n", dev_desc->idProduct));

    /* get configuration descriptor head */
    ret = rt_usb_get_descriptor(uinst, USB_DESC_TYPE_CONFIGURATION, &cfg_desc, 
        sizeof(struct uconfig_descriptor));
    if(ret != RT_EOK)
    {
        rt_kprintf("get configuration descriptor head failed\n");
        return ret;
    }

    /* alloc memory for configuration descriptor */
    uinst->cfg_desc = (ucfg_desc_t)rt_malloc(cfg_desc.wTotalLength);
    rt_memset(uinst->cfg_desc, 0, cfg_desc.wTotalLength);

    /* get full configuration descriptor */
    ret = rt_usb_get_descriptor(uinst, USB_DESC_TYPE_CONFIGURATION, 
        uinst->cfg_desc, cfg_desc.wTotalLength);
    if(ret != RT_EOK)
    {
        rt_kprintf("get full configuration descriptor failed\n");
        return ret;
    }

    /* set configuration */
    ret = rt_usb_set_configure(uinst, 1);
    if(ret != RT_EOK) return ret;

    for(i=0; i<uinst->cfg_desc->bNumInterfaces; i++)
    {        
        /* get interface descriptor through configuration descriptor */
        ret = rt_usb_get_interface_descriptor(uinst->cfg_desc, i, &intf_desc);
        if(ret != RT_EOK)
        {
            rt_kprintf("rt_usb_get_interface_descriptor error\n");
            return -RT_ERROR;
        }

        RT_DEBUG_LOG(RT_DEBUG_USB, ("interface class 0x%x, subclass 0x%x\n", 
                                    intf_desc->bInterfaceClass,
                                    intf_desc->bInterfaceSubClass));

        /* find driver by class code found in interface descriptor */
        drv = rt_usb_class_driver_find(intf_desc->bInterfaceClass, 
            intf_desc->bInterfaceSubClass);
        
        if(drv != RT_NULL)
        {
            /* allocate memory for interface uinst */
            uinst->ifinst[i] = 
                (uifinst_t)rt_malloc(sizeof(struct uifinst));

            uinst->ifinst[i]->drv = drv;
            uinst->ifinst[i]->uinst = uinst;
            uinst->ifinst[i]->intf_desc = intf_desc;
            uinst->ifinst[i]->user_data = RT_NULL;

            /* open usb class driver */
            ret = rt_usb_class_driver_run(drv, (void*)uinst->ifinst[i]);
            if(ret != RT_EOK)
            {
                rt_kprintf("interface %d run class driver error\n", i);
            }
        }
        else
        {
            rt_kprintf("find usb device driver failed\n");
            continue;
        }
    }
    
    return RT_EOK;
}

/**
 * This function will detach an usb device instance from its host controller,
 * and release all resource.
 *
 * @param uinst the usb device instance.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_detach_instance(uinst_t uinst)
{
    int i = 0;

    if(uinst == RT_NULL) 
    {
        rt_kprintf("no usb instance to detach\n");
        return -RT_ERROR;
    }
    
    /* free configration descriptor */
    if(uinst->cfg_desc) rt_free(uinst->cfg_desc);
    
    for(i=0; i<uinst->cfg_desc->bNumInterfaces; i++)
    {
        if(uinst->ifinst[i] == RT_NULL) continue;
        if(uinst->ifinst[i]->drv == RT_NULL) continue;

        RT_ASSERT(uinst->ifinst[i]->uinst == uinst);

        RT_DEBUG_LOG(RT_DEBUG_USB, ("free interface instance %d\n", i));
        rt_usb_class_driver_stop(uinst->ifinst[i]->drv, (void*)uinst->ifinst[i]);
    }
    
    rt_memset(uinst, 0, sizeof(struct uinstance));
    
    return RT_EOK;
}

/**
 * This function will do USB_REQ_GET_DESCRIPTO' request for the usb device instance,
 *
 * @param uinst the usb device instance. 
 * @param type the type of descriptor request.
 * @param buffer the data buffer to save requested data
 * @param nbytes the size of buffer
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_get_descriptor(uinst_t uinst, rt_uint8_t type, void* buffer, 
    int nbytes)
{
    struct ureqest setup;
    int timeout = 100;
    
    RT_ASSERT(uinst != RT_NULL);

    setup.request_type = USB_REQ_TYPE_DIR_IN | USB_REQ_TYPE_STANDARD | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_GET_DESCRIPTOR;
    setup.index = 0;
    setup.length = nbytes;
    setup.value = type << 8;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, buffer, nbytes, 
        timeout) != nbytes) return -RT_EIO;
    else return RT_EOK;
}

/**
 * This function will set an address to the usb device.
 *
 * @param uinst the usb device instance.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_set_address(uinst_t uinst)
{
    struct ureqest setup;
    int timeout = 100;
    
    RT_ASSERT(uinst != RT_NULL);

    RT_DEBUG_LOG(RT_DEBUG_USB, ("rt_usb_set_address\n"));

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_STANDARD | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_SET_ADDRESS;
    setup.index = 0;
    setup.length = 0;
    setup.value = uinst->index;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, RT_NULL, 0, 
        timeout) != 0) return -RT_EIO;

    rt_thread_delay(50);

    uinst->address = uinst->index;
    
    return RT_EOK;
}

/**
 * This function will set a configuration to the usb device.
 *
 * @param uinst the usb device instance.
 * @param config the configuration number.
  * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_set_configure(uinst_t uinst, int config)
{
    struct ureqest setup;
    int timeout = 100;

    /* check parameter */
    RT_ASSERT(uinst != RT_NULL);

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_STANDARD | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_SET_CONFIGURATION;
    setup.index = 0;
    setup.length = 0;
    setup.value = config;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, RT_NULL, 0, 
        timeout) != 0) return -RT_EIO;
    
    return RT_EOK;    
}

/**
 * This function will set an interface to the usb device.
 *
 * @param uinst the usb device instance.
 * @param intf the interface number.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_set_interface(uinst_t uinst, int intf)
{
    struct ureqest setup;
    int timeout = 100;

    /* check parameter */
    RT_ASSERT(uinst != RT_NULL);

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_STANDARD | 
        USB_REQ_TYPE_INTERFACE;
    setup.request = USB_REQ_SET_INTERFACE;
    setup.index = 0;
    setup.length = 0;
    setup.value = intf;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, RT_NULL, 0, 
        timeout) != 0) return -RT_EIO;
    
    return RT_EOK;    
}

/**
 * This function will clear feature for the endpoint of the usb device.
 *
 * @param uinst the usb device instance.
 * @param endpoint the endpoint number of the usb device.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_clear_feature(uinst_t uinst, int endpoint, int feature)
{
    struct ureqest setup;
    int timeout = 100;

    /* check parameter */
    RT_ASSERT(uinst != RT_NULL);

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_STANDARD | 
        USB_REQ_TYPE_ENDPOINT;
    setup.request = USB_REQ_CLEAR_FEATURE;
    setup.index = endpoint;
    setup.length = 0;
    setup.value = feature;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, RT_NULL, 0, 
        timeout) != 0) return -RT_EIO;
    
    return RT_EOK;    
}

/**
 * This function will get an interface descriptor from the configuration descriptor.
 *
 * @param cfg_desc the point of configuration descriptor structure.
 * @param num the number of interface descriptor.
 * @intf_desc the point of interface descriptor point.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_get_interface_descriptor(ucfg_desc_t cfg_desc, int num, 
    uintf_desc_t* intf_desc)
{
    rt_uint32_t ptr, depth = 0;
    udesc_t desc;

    /* check parameter */
    RT_ASSERT(cfg_desc != RT_NULL);

    ptr = (rt_uint32_t)cfg_desc + cfg_desc->bLength;
    while(ptr < (rt_uint32_t)cfg_desc + cfg_desc->wTotalLength)
    {
        if(depth++ > 0x20) 
        {
            *intf_desc = RT_NULL;        
            return -RT_EIO;
        }
        desc = (udesc_t)ptr;
        if(desc->type == USB_DESC_TYPE_INTERFACE)
        {
            if(((uintf_desc_t)desc)->bInterfaceNumber == num)
            {
                *intf_desc = (uintf_desc_t)desc;

                RT_DEBUG_LOG(RT_DEBUG_USB,
                             ("rt_usb_get_interface_descriptor: %d\n", num));                
                return RT_EOK;
            }
        }    
        ptr = (rt_uint32_t)desc + desc->bLength;
    }

    rt_kprintf("rt_usb_get_interface_descriptor %d failed\n", num);
    return -RT_EIO;
}

/**
 * This function will get an endpoint descriptor from the interface descriptor.
 *
 * @param intf_desc the point of interface descriptor structure.
 * @param num the number of endpoint descriptor.
 * @param ep_desc the point of endpoint descriptor point.
 * 
 * @return the error code, RT_EOK on successfully.
 */
rt_err_t rt_usb_get_endpoint_descriptor(uintf_desc_t intf_desc, int num, 
    uep_desc_t* ep_desc)
{
    int count = 0, depth = 0;
    rt_uint32_t ptr;    
    udesc_t desc;

    /* check parameter */
    RT_ASSERT(intf_desc != RT_NULL);
    RT_ASSERT(num < intf_desc->bNumEndpoints);

    ptr = (rt_uint32_t)intf_desc + intf_desc->bLength;
    while(count < intf_desc->bNumEndpoints)
    {
        if(depth++ > 0x20) 
        {
            *ep_desc = RT_NULL;            
            return -RT_EIO;
        }
        desc = (udesc_t)ptr;        
        if(desc->type == USB_DESC_TYPE_ENDPOINT)
        {
            if(num == count) 
            {
                *ep_desc = (uep_desc_t)desc;

                RT_DEBUG_LOG(RT_DEBUG_USB,
                             ("rt_usb_get_endpoint_descriptor: %d\n", num));
                return RT_EOK;
            }
            else count++;
        }
        ptr = (rt_uint32_t)desc + desc->bLength;
    }

    rt_kprintf("rt_usb_get_endpoint_descriptor %d failed\n", num);
    return -RT_EIO;
}

