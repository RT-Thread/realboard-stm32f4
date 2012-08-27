#include <rtthread.h>
#include "usb_common.h"

#define UDEVICE_MAX         2
#define UCLASS_MAX          2
#define UINTERFACE_MAX      2
#define UENDPOINT_MAX       4

struct udevice;

struct udcd_ops
{
    rt_err_t (*set_address)(rt_uint8_t value);
    rt_err_t (*clear_feature)(rt_uint8_t value);
    rt_err_t (*set_feature)(rt_uint8_t value);
};

struct udcd
{
    struct rt_device parent;
    struct udcd_ops* ops;
};
typedef struct udcd* udcd_t;

struct uendpoint
{
    uep_desc_t ep_desc;
    rt_err_t (*handler)(struct udevice* device, ureq_t setup);    
};
typedef struct uendpoint* uep_t;

struct uinterface
{
    uintf_desc_t intf_desc;
    rt_err_t (*handler)(struct udevice* device, ureq_t setup);    

    rt_uint8_t ep_nr;
    struct uendpoint* ep[UENDPOINT_MAX];
};
typedef struct uinterface* uintf_t; 

struct uclass
{
    rt_err_t (*handler)(struct udevice* device, ureq_t setup);    
    
    rt_uint8_t intf_nr;
    struct uinterface* intf[UINTERFACE_MAX];
};
typedef struct uclass* uclass_t;

struct udevice
{
    udcd_t dcd;    
    rt_bool_t status;
     
    udev_desc_t dev_desc;
    ucfg_desc_t cfg_desc;    
    ustr_desc_t lang_desc;
    ustr_desc_t manu_desc;
    ustr_desc_t prod_desc;
    ustr_desc_t sn_desc;

    rt_uint8_t class_nr;
    struct uclass *uclass[UCLASS_MAX];
};
typedef struct udevice* udevice_t;

enum udev_msg_type
{
    USB_MSG_SETUP_NOTIFY,
    USB_MSG_DATA_NOTIFY,
};
typedef enum udev_msg_type udev_msg_type;

struct udev_msg
{
    udev_msg_type type; 
    union
    {
        struct
        {
            udcd_t dcd;
            rt_uint8_t* packet;
        }setup_msg;
        struct 
        {
            udcd_t dcd;
            rt_uint8_t ep_addr;
        }ep_msg;
    }content;
};
typedef struct udev_msg* udev_msg_t;

rt_err_t rt_usb_device_init(void);

udevice_t rt_usbd_alloc_device(void);
udevice_t rt_usbd_find_device(udcd_t dcd);
rt_err_t rt_usbd_free_device(udevice_t device);

uclass_t rt_usb_class_mass(void);

rt_inline rt_err_t dcd_set_address(udcd_t dcd, rt_uint8_t value)
{
    RT_ASSERT(dcd != RT_NULL);
    
    return dcd->ops->set_address(value);
}

rt_inline rt_err_t dcd_clear_feature(udcd_t dcd, rt_uint8_t value)
{
    RT_ASSERT(dcd != RT_NULL);
    
    return dcd->ops->clear_feature(value);
}

rt_inline rt_err_t dcd_set_feature(udcd_t dcd, rt_uint8_t value)
{
    RT_ASSERT(dcd != RT_NULL);
    
    return dcd->ops->set_feature(value);
}

