/*
 * File      : adk.c
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
#include "adk.h"

#ifdef RT_USB_CLASS_ADK

static struct uclass_driver adk_driver;
static const char* _adk_manufacturer = RT_NULL;
static const char* _adk_model = RT_NULL;
static const char* _adk_description = RT_NULL;
static const char* _adk_version = RT_NULL;
static const char* _adk_uri = RT_NULL;
static const char* _adk_serial = RT_NULL;

rt_err_t rt_usb_adk_set_string(const char* manufacturer, const char* model,
    const char* description, const char* _version, const char* uri, 
    const char* serial)
{
    _adk_manufacturer = manufacturer;
    _adk_model = model;
    _adk_description = description;
    _adk_version = _version;
    _adk_uri = uri;
    _adk_serial = serial;

    return RT_EOK;
}

#ifdef RT_USING_MODULE
#include <rtm.h>

RTM_EXPORT(rt_usb_adk_set_string);
#endif

/**
 * This function will do USB_REQ_GET_PROTOCOL request to set idle period to the usb adk device
 *
 * @param ifinst the interface instance.
 * @duration the idle period of requesting data.
 * @report_id the report id
 * 
 * @return the error code, RT_EOK on successfully.
*/
static rt_err_t rt_usb_adk_get_protocol(uifinst_t ifinst, rt_uint16_t *protocol)
{
    struct ureqest setup;
    uinst_t uinst;    
    int timeout = 100;
            
        /* parameter check */
    RT_ASSERT(ifinst != RT_NULL);
    RT_ASSERT(ifinst->uinst != RT_NULL);

    uinst = ifinst->uinst;

    setup.request_type = USB_REQ_TYPE_DIR_IN | USB_REQ_TYPE_VENDOR | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_GET_PROTOCOL;
    setup.index = 0;
    setup.length = 2;
    setup.value = 0;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, (void*)protocol, 2, 
        timeout) == 0) return RT_EOK;
    else return -RT_FALSE;    
}

/**
 * This function will do USB_REQ_SEND_STRING request to set idle period to the usb adk device
 *
 * @param ifinst the interface instance.
 * @duration the idle period of requesting data.
 * @report_id the report id
 * 
 * @return the error code, RT_EOK on successfully.
*/
static rt_err_t rt_usb_adk_send_string(uifinst_t ifinst, rt_uint16_t index, 
    const char* str)
{
    struct ureqest setup;
    uinst_t uinst;    
    int timeout = 100;
            
        /* parameter check */
    RT_ASSERT(ifinst != RT_NULL);
    RT_ASSERT(ifinst->uinst != RT_NULL);

    uinst = ifinst->uinst;

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_VENDOR | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_SEND_STRING;
    setup.index = index;
    setup.length = rt_strlen(str) + 1;
    setup.value = 0;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, (void*)str, 
        rt_strlen(str) + 1, timeout) == 0) return RT_EOK;
    else return -RT_FALSE;   
}

/**
 * This function will do USB_REQ_START request to set idle period to the usb adk device
 *
 * @param ifinst the interface instance.
 * @duration the idle period of requesting data.
 * @report_id the report id
 * 
 * @return the error code, RT_EOK on successfully.
*/
static rt_err_t rt_usb_adk_start(uifinst_t ifinst)
{
    struct ureqest setup;
    uinst_t uinst;    
    int timeout = 100;
            
        /* parameter check */
    RT_ASSERT(ifinst != RT_NULL);
    RT_ASSERT(ifinst->uinst != RT_NULL);

    uinst = ifinst->uinst;

    setup.request_type = USB_REQ_TYPE_DIR_OUT | USB_REQ_TYPE_VENDOR | 
        USB_REQ_TYPE_DEVICE;
    setup.request = USB_REQ_START;
    setup.index = 0;
    setup.length = 0;
    setup.value = 0;

    if(rt_usb_hcd_control_xfer(uinst->hcd, uinst, &setup, RT_NULL, 0, 
        timeout) == 0) return RT_EOK;
    else return -RT_FALSE;   
}

/**
 * This function will read data from usb adk device
 *
 * @param ifinst the interface instance.
 * 
 * @return the error code, RT_EOK on successfully.
*/
static rt_size_t rt_usb_adk_read(rt_device_t device, rt_off_t pos, void* buffer, 
    rt_size_t size)
{
    uadkinst_t adkinst;
    rt_size_t length;
    uifinst_t ifinst;

    /* check parameter */
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(buffer != RT_NULL);

    ifinst = (uifinst_t)device->user_data;
    adkinst = (uadkinst_t)ifinst->user_data;

    length = rt_usb_hcd_bulk_xfer(ifinst->uinst->hcd, adkinst->pipe_in, 
        buffer, size, 300);
    
    return length;

}

/**
 * This function will write data to usb adk device
 *
 * @param ifinst the interface instance.
 * 
 * @return the error code, RT_EOK on successfully.
*/
static rt_size_t rt_usb_adk_write (rt_device_t device, rt_off_t pos, const void* buffer, 
    rt_size_t size)
{
    uadkinst_t adkinst;
    rt_size_t length;
    uifinst_t ifinst;

    RT_ASSERT(buffer != RT_NULL);    

    ifinst = (uifinst_t)device->user_data;
    adkinst = (uadkinst_t)ifinst->user_data;

    length = rt_usb_hcd_bulk_xfer(ifinst->uinst->hcd, adkinst->pipe_out, 
        (void*)buffer, size, 300);
    
    return length;
}

/**
 * This function will run adk class driver when usb device is detected and identified
 *  as a adk class device, it will continue the enumulate process.
 *
 * @param arg the argument.
 * 
 * @return the error code, RT_EOK on successfully.
 */
static rt_err_t rt_usb_adk_run(void* arg)
{
    int i = 0;
    uadkinst_t adkinst;
    uifinst_t ifinst = (uifinst_t)arg;
    udev_desc_t dev_desc;
    rt_uint16_t protocol;
    rt_err_t ret;    
    
    /* parameter check */
    if(ifinst == RT_NULL)
    {
        rt_kprintf("the interface is not available\n");
        return -RT_EIO;
    }

    RT_DEBUG_LOG(RT_DEBUG_USB, ("rt_usb_adk_run\n"));
        
    dev_desc = &ifinst->uinst->dev_desc;
    if(dev_desc->idVendor == USB_ACCESSORY_VENDOR_ID && 
        (dev_desc->idProduct == USB_ACCESSORY_PRODUCT_ID || 
        dev_desc->idProduct == USB_ACCESSORY_ADB_PRODUCT_ID))
    {
        if(ifinst->intf_desc->bInterfaceSubClass != 0xFF) return -RT_ERROR;
    
        RT_DEBUG_LOG(RT_DEBUG_USB, ("found android accessory device\n"));        
    }
    else
    {
        RT_DEBUG_LOG(RT_DEBUG_USB, ("switch device\n"));        
        
        if((ret = rt_usb_adk_get_protocol(ifinst, &protocol)) != RT_EOK)
        {
            rt_kprintf("rt_usb_adk_get_protocol failed\n");
            return ret;
        }

        if(protocol != 1) 
        {
            rt_kprintf("read protocol failed\n");
            return -RT_ERROR;
        }       

        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_MANUFACTURER, _adk_manufacturer);
        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_MODEL, _adk_model);
        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_DESCRIPTION, _adk_description);
        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_VERSION, _adk_version);
        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_URI, _adk_uri);        
        rt_usb_adk_send_string(ifinst, 
            ACCESSORY_STRING_SERIAL, _adk_serial);            

        RT_DEBUG_LOG(RT_DEBUG_USB, ("manufacturer %s\n", _adk_manufacturer));
        RT_DEBUG_LOG(RT_DEBUG_USB, ("model %s\n", _adk_model));
        RT_DEBUG_LOG(RT_DEBUG_USB, ("description %s\n", _adk_description));
        RT_DEBUG_LOG(RT_DEBUG_USB, ("version %s\n", _adk_version));
        RT_DEBUG_LOG(RT_DEBUG_USB, ("uri %s\n", _adk_uri));       
        RT_DEBUG_LOG(RT_DEBUG_USB, ("serial %s\n", _adk_serial));               
        
        if((ret = rt_usb_adk_start(ifinst)) != RT_EOK)
        {
            rt_kprintf("rt_usb_adk_start failed\n");
            return ret;
        }        

        return RT_EOK;
    }
    
    adkinst = rt_malloc(sizeof(struct uadkinst));
    RT_ASSERT(adkinst != RT_NULL);

    /* initilize the data structure */
    rt_memset(adkinst, 0, sizeof(struct uadkinst));    
    ifinst->user_data = (void*)adkinst;

    for(i=0; i<ifinst->intf_desc->bNumEndpoints; i++)
    {        
        uep_desc_t ep_desc;
        
        /* get endpoint descriptor from interface descriptor */
        rt_usb_get_endpoint_descriptor(ifinst->intf_desc, i, &ep_desc);
        if(ep_desc == RT_NULL)
        {
            rt_kprintf("rt_usb_get_endpoint_descriptor error\n");
            return -RT_ERROR;
        }
        
        /* the endpoint type of adk class should be BULK */    
        if((ep_desc->bmAttributes & USB_EP_ATTR_TYPE_MASK) != USB_EP_ATTR_BULK)
            continue;
        
        /* allocate pipes according to the endpoint type */
        if(ep_desc->bEndpointAddress & USB_DIR_IN)
        {
            /* allocate an in pipe for the adk instance */
            ret = rt_usb_hcd_alloc_pipe(ifinst->uinst->hcd, &adkinst->pipe_in, 
                ifinst, ep_desc, RT_NULL);
            if(ret != RT_EOK) return ret;
        }
        else
        {        
            /* allocate an output pipe for the adk instance */
            ret = rt_usb_hcd_alloc_pipe(ifinst->uinst->hcd, &adkinst->pipe_out, 
                ifinst, ep_desc, RT_NULL);            
            if(ret != RT_EOK) return ret;
        }
    }

    /* check pipes infomation */
    if(adkinst->pipe_in == RT_NULL || adkinst->pipe_out == RT_NULL)
    {
        rt_kprintf("pipe error, unsupported device\n");
        return -RT_ERROR;
    }    

    /* set configuration */
    ret = rt_usb_set_configure(ifinst->uinst, 1);
    if(ret != RT_EOK) return ret;

    /* register adk device */
    adkinst->device.type  = RT_Device_Class_Char;                         
    adkinst->device.init = RT_NULL;         
    adkinst->device.open = RT_NULL;         
    adkinst->device.close = RT_NULL;                 
    adkinst->device.read = rt_usb_adk_read;
    adkinst->device.write = rt_usb_adk_write;
    adkinst->device.control = RT_NULL;
    adkinst->device.user_data = (void*)ifinst;

    rt_device_register(&adkinst->device, "adkdev", RT_DEVICE_FLAG_RDWR);
    
    return RT_EOK;
}

/**
 * This function will be invoked when usb device plug out is detected and it would clean 
 * and release all hub class related resources.
 *
 * @param arg the argument.
 * 
 * @return the error code, RT_EOK on successfully.
 */
static rt_err_t rt_usb_adk_stop(void* arg)
{
    uadkinst_t adkinst;
    uifinst_t ifinst = (uifinst_t)arg;

    RT_ASSERT(ifinst != RT_NULL);

    RT_DEBUG_LOG(RT_DEBUG_USB, ("rt_usb_adk_stop\n"));

    adkinst = (uadkinst_t)ifinst->user_data;
    if(adkinst == RT_NULL) 
    {
        rt_free(ifinst);    
        return RT_EOK;
    }
    
    if(adkinst->pipe_in != RT_NULL)
        rt_usb_hcd_free_pipe(ifinst->uinst->hcd, adkinst->pipe_in);

    if(adkinst->pipe_out != RT_NULL)
        rt_usb_hcd_free_pipe(ifinst->uinst->hcd, adkinst->pipe_out);

    /* unregister adk device */
    rt_device_unregister(&adkinst->device);

    /* free adk instance */
    if(adkinst != RT_NULL) rt_free(adkinst);
    
    /* free interface instance */
    rt_free(ifinst);

    return RT_EOK;
}

/**
 * This function will register adk class driver to the usb class driver manager.
 * and it should be invoked in the usb system initialization.
 * 
 * @return the error code, RT_EOK on successfully.
 */
ucd_t rt_usb_class_driver_adk(void)
{
    adk_driver.class_code = USB_CLASS_ADK;
    
    adk_driver.run = rt_usb_adk_run;
    adk_driver.stop = rt_usb_adk_stop;

    return &adk_driver;
}

#endif

