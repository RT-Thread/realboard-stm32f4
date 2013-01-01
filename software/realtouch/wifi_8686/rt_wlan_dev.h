#ifndef __RT_WLAN_DEV_H__
#define __RT_WLAN_DEV_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <netif/ethernetif.h>

#include "wlan_types.h"

#define SSID_NAME_MAX           16
#define PASSWORD_LENGTH_MAX     32
#define MAC_LENGTH_MAX          6

#define RT_WLAN_DEVICE(device)  ((struct rt_wlan_device*)(device))

#define WLAN_STATUS_CONNECTED   0
#define WLAN_STATUS_CONNECTING  1
#define WLAN_STATUS_IDLE        2
#define WLAN_STATUS_SUSPENDED   3

#define WLAN_MODE_INFRA         0
#define WLAN_MODE_ADHOC         1
#define WLAN_MODE_SOFTAP        2

#define WLAN_SECURITY_OPEN      0
#define WLAN_SECURITY_WEP       1
#define WLAN_SECURITY_WPA       2
#define WLAN_SECURITY_WPA2      3

#define WLAN_CTRL_SET_SSID      0x10
#define WLAN_CTRL_SET_PASSWORD  0x11
#define WLAN_CTRL_SET_CHANNEL   0x12
#define WLAN_CTRL_SET_MODE      0x13
#define WLAN_CTRL_SET_SECURITY  0x14
#define WLAN_CTRL_SET_BSADDR	0x15
#define WLAN_CTRL_GET_RSSI      0x16

struct rt_access_point
{
    rt_uint16_t rssi;
    rt_uint8_t security;
    rt_uint8_t channel;

    char ssid[SSID_NAME_MAX];
    rt_uint8_t mac_addr[MAC_LENGTH_MAX];
};

struct rt_wlan_device;
struct rt_wlan_device_ops
{
    rt_err_t (*probe)     (struct rt_wlan_device* device);

    rt_err_t (*suspend)   (struct rt_wlan_device* device);
    rt_err_t (*wakeup)    (struct rt_wlan_device* device);

    rt_err_t (*scan)      (struct rt_wlan_device* device, struct rt_access_point* aps, rt_size_t size);
    rt_err_t (*associate) (struct rt_wlan_device* device);
    rt_err_t (*disconnect)(struct rt_wlan_device* device);
};

struct rt_wlan_device
{
    struct eth_device parent;

    char ssid[SSID_NAME_MAX];
    char password[PASSWORD_LENGTH_MAX];
    rt_uint32_t rssi;
    rt_uint8_t bs_addr[MAC_LENGTH_MAX];		/* base station MAC address */

    rt_uint8_t status;
    rt_uint8_t mode;
    rt_uint8_t security;
    rt_uint8_t channel;
    
    const struct rt_wlan_device_ops* ops;
};

rt_inline rt_err_t rt_wlan_suspend(struct rt_wlan_device* device)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->ops != RT_NULL);

    if (device->status == WLAN_STATUS_SUSPENDED) return result;

    if (device->ops->suspend) result = device->ops->suspend(device);

    return result;
}

rt_inline rt_err_t rt_wlan_wakeup(struct rt_wlan_device* device)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->ops != RT_NULL);

    if (device->status != WLAN_STATUS_SUSPENDED) return result;

    if (device->ops->wakeup) result = device->ops->wakeup(device);

    return result;
}

rt_inline rt_err_t rt_wlan_scan(struct rt_wlan_device* device, struct rt_access_point* aps, rt_size_t size)
{
    rt_err_t result = -RT_ERROR;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->ops != RT_NULL);

    if (device->ops->scan) result = device->ops->scan(device, aps, size);

    return result;
}

rt_inline rt_err_t rt_wlan_disconnect(struct rt_wlan_device* device)
{
    rt_err_t result = -RT_ERROR;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->ops != RT_NULL);

    if (device->ops->disconnect) result = device->ops->disconnect(device);

    return result;
}

rt_inline rt_err_t rt_wlan_associate(struct rt_wlan_device* device)
{
    rt_err_t result = -RT_ERROR;

    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->ops != RT_NULL);

    if (device->status == WLAN_STATUS_CONNECTING) return -RT_EBUSY;
    if (device->status == WLAN_STATUS_CONNECTED)
    {
        /* disconnect the old association */
        rt_wlan_disconnect(device);
    }
    if (device->ops->associate) result = device->ops->associate(device);

    return result;
}

rt_err_t rt_wlan_device_init(struct rt_wlan_device* device, const struct rt_wlan_device_ops* ops, char* name);
rt_err_t rt_wlan_device_control(struct rt_wlan_device* device, rt_uint8_t cmd, void* arg);

typedef struct multi_addr_struct
{
	rt_uint8_t multi_addr[MRVDRV_MAX_MULTICAST_LIST_SIZE][MRVDRV_ETH_ADDR_LEN];
	rt_uint8_t addressnum;
} Multi_Addr_Struct;

struct rt_wlan_dev
{
    /* inherit from wlan device */
	struct rt_wlan_device parent;

    void *priv ;

    unsigned int irq;
    unsigned int flags;	/* interface flags (BSD)	*/
};

/* initialize marvell wlan hardware */
rt_err_t mrvl_wlan_hw_init(const char* spi_device);

/* get WlanCard from a Marvell wifi device */
#define WLAN_CARD(device) ((WlanCard*)(((WlanInfo*)(((struct rt_wlan_dev*)device)->priv))->card))

#endif

