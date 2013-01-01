#include <rtthread.h>
#include <netif/ethernetif.h>

#include "rt_wlan_dev.h"
#include "wlan_dev.h"
#include "sbi.h"
#include "if_gspi.h"

#include <dfs_posix.h>

extern int WlanInitPhase2(void);
extern int WlanInitPhase1(struct rt_wlan_dev* RTWlanDev, const char* spi_device);

/* wlan ethernet network device */
struct rt_wlan_dev wlan_eth;

/* RT-Thread Device Interface */
/* initialize the interface */
static rt_err_t mrvl_wlan_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t mrvl_wlan_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t mrvl_wlan_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t mrvl_wlan_write(rt_device_t dev, rt_off_t pos,
	const void *buffer, rt_size_t size)
{
	struct eth_device* eth;
	struct pbuf* p;

	eth = (struct eth_device*)dev;
	RT_ASSERT(eth != RT_NULL);

	p = pbuf_alloc(PBUF_LINK, size, PBUF_RAM);
	if (p == RT_NULL) return 0;

	pbuf_take(p, buffer, size);

	eth->netif->linkoutput(eth->netif, p);

	return size;
}

rt_err_t mrvl_wlan_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	WlanCard *card;
	struct rt_wlan_device *device;

    RT_ASSERT(dev != RT_NULL);

	card = WLAN_CARD(dev);
	device = (struct rt_wlan_device*) dev;

	switch (cmd)
	{
	case NIOCTL_GADDR:
		/* get MAC Address of card */
        rt_memcpy((rt_uint8_t*)args, card->MyMacAddress, 6);
		return RT_EOK;

	default :
		return rt_wlan_device_control(device, cmd, args);
	}

	return -RT_ERROR;
}

/* ethernet device interface */
static rt_err_t mrvl_wlan_tx( rt_device_t dev, struct pbuf* packet)
{
	WlanCard *card;
	rt_uint32_t level;

	RT_ASSERT(dev != RT_NULL);
	RT_ASSERT(packet != RT_NULL);

	card = WLAN_CARD(dev);

    SendSinglePacket(card, packet);

    level = rt_hw_interrupt_disable();
    card->HisRegCpy &= ~HIS_TxDnLdRdy;
    rt_hw_interrupt_enable(level);

    return RT_EOK;
}

/* reception packet. */
static struct pbuf *mrvl_wlan_rx(rt_device_t dev)
{
	WlanCard *card;
	Rx_Pbuf_List *RxListHead;
	struct pbuf* p;
	rt_base_t level;

	RT_ASSERT(dev != RT_NULL);

	card = WLAN_CARD(dev);

__again:
	p = RT_NULL;
	RxListHead = RT_NULL;

	level = rt_hw_interrupt_disable();
	if (card->RxList.next != &card->RxList)
	{
		RxListHead=card->RxList.next;
		p=RxListHead->p;
		card->RxList.next=RxListHead->next;
		RxListHead->next->pre=RxListHead->pre;
		card->RxQueueCount-- ;
	}
	rt_hw_interrupt_enable(level);

	if (RxListHead != RT_NULL)
	{
		rt_free(RxListHead);
		RxListHead = RT_NULL;
	}

	if ((p != RT_NULL) && (is_ieee802_11(p) == RT_TRUE))
		goto __again;

	if (p != RT_NULL)
	{
		hexdump("Rx Packet: \n", p->payload, p->len);
	}

    return p;
}

static rt_err_t mrvl_wlan_probe(struct rt_wlan_device* device)
{
	rt_err_t result = RT_EOK;
	rt_uint8_t chiprev = 0xff;
	
    gspi_read_reg(CHIPREV_REG, &chiprev);
	if (chiprev == 0xff) return -RT_ERROR;

	return RT_EOK;
}

static rt_err_t mrvl_wlan_suspend(struct rt_wlan_device* device)
{
	rt_err_t result = RT_EOK;

	return RT_EOK;
}

static rt_err_t mrvl_wlan_wakeup(struct rt_wlan_device* device)
{
	rt_err_t result = RT_EOK;

	return RT_EOK;
}

static rt_err_t mrvl_wlan_scan(struct rt_wlan_device* device, struct rt_access_point* aps, rt_size_t size)
{
	rt_err_t result = RT_EOK;

	return RT_EOK;
}

#define SCAN_BUFSZ		(1024 * 2)
static rt_err_t mrvl_scan_card(struct rt_wlan_device* device, WlanConfig *config)
{
	rt_err_t result = RT_EOK;
	WlanCard *card;
	ScanResultItem * item;
	int ret, i;
	rt_uint8_t *scan_buf;

	RT_ASSERT(device != RT_NULL);
	card = WLAN_CARD(device);

	scan_buf = (rt_uint8_t*)rt_calloc(1, SCAN_BUFSZ);
	if (scan_buf == NULL)
	{
		WlanDebug(WlanErr,"WlanScan:alloc memory failed\r\n");
		return -RT_ERROR;
	}

	ret = SendSpecificScanConfig(card, config, scan_buf, SCAN_BUFSZ);
	if (ret != 0)
	{
		WlanDebug(WlanErr,"Filter channels scan failed\r\n");
		return -RT_ERROR;
	}

	if (card->ScanResultcount == 0)
	{
		ret = SendScanToGetAPInfo(card, scan_buf, SCAN_BUFSZ);
		if (ret != 0)
		{
			WlanDebug(WlanErr,"All channels scan failed\r\n");
			return -RT_ERROR;
		}
	}

	WlanDebug(WlanErr,"AP totaly %d\r\n",card->ScanResultcount);
	for (i = 0; i < card->ScanResultcount; i++)
	{
		item = card->ScanResultInfo + i;
		rt_kprintf("AP[%d]: %s  ", i + 1, item->Ssid.Ssid);
		rt_kprintf("BSSID:%x-%x-%x-%x-%x-%x ", item->MacAddress[0],
				item->MacAddress[1], item->MacAddress[2],
				item->MacAddress[3], item->MacAddress[4],
				item->MacAddress[5]);
		rt_kprintf("Channel:%d ", item->Channel);
		rt_kprintf("RSSI:0x%x ", item->Rssi);
		rt_kprintf("Privacy:0x%x ", item->Privacy);
		switch (item->Security)
		{
		case WEP:
			rt_kprintf("[WEP ]");
			break;
		case WPA_PSK:
			rt_kprintf("[WPA ]");
			break;
		case WPA2_PSK:
			rt_kprintf("[WPA2]");
			break;
		case NoSecurity:
			rt_kprintf("[NONE]");
		default:
			break;
		}
		rt_kprintf("\r\n");

		if (rt_strcmp(item->Ssid.Ssid, config->SSID) == 0)
		{
			/* set bs address and channel */
			config->channel = item->Channel;
			config->security = item->Security;
			if(item->Privacy == 0)
            {
                if(item->Security == WEP)
                {
                    config->security = NoSecurity;
                }
                else
                {
                    WlanDebug(WlanErr, "Privacy = 0 but Security != WEP\r\n");
                }
            }
			
			rt_memcpy(config->MacAddr, item->MacAddress, sizeof(item->MacAddress));
			break;
		}
	}

	/* release scan buffer */
	rt_free(scan_buf);

	card->ScanResulMuxsize=0;
	card->ScanResultInfo = RT_NULL;
	card->ScanResultcount=0;
}

static rt_err_t mrvl_wlan_associate(struct rt_wlan_device* device)
{
	WlanCard *card;
	rt_err_t result = RT_EOK;
	WlanConfig *config;

	RT_ASSERT(device != RT_NULL);
	card = WLAN_CARD(device);

	if (device->ssid[0] == '\0') return -RT_ERROR;

	/* set mode */ 
	if (device->mode == WLAN_MODE_INFRA)
		card->InfrastructureMode = Wlan802_11Infrastructure;
	else
		card->InfrastructureMode = Wlan802_11IBSS;
	wlan_set_infrastructure(card);


	config = (WlanConfig*) rt_calloc(1, sizeof(WlanConfig));
	if (config == RT_NULL) return -RT_ENOMEM;

	config->channel = device->channel;
	/* copy SSID and base station address */
	rt_strncpy(config->SSID, device->ssid, SSID_NAME_MAX);
	memcpy(config->MacAddr, device->bs_addr, MAC_LENGTH_MAX);
	
	if (device->password[0] == '\0') config->security = NoSecurity;
	else 
	{
		switch (device->security)
		{
		case WLAN_SECURITY_WEP:
			config->security = WEP;
			break;
		case WLAN_SECURITY_WPA:
			config->security = WPA_PSK;
			break;
		case WLAN_SECURITY_WPA2:
			config->security = WPA2_PSK;
			break;
		default:
			config->security = 0;
			break;
		}

		rt_strncpy(config->password, device->password, PASSWORD_LENGTH_MAX);
	}

	if (device->channel == 0xff)
	{
		/* try to scan AP */
		mrvl_scan_card(device, config);
	}

	if (config->channel == 0xff) 
	{
		rt_kprintf("not found AP\n");
		return -RT_ERROR;
	}

	/* set wlan status */
	device->status = WLAN_STATUS_CONNECTING;

	result = wlan_cmd_802_11_associate_cfg(card, config);
	if (result != WLAN_STATUS_SUCCESS)
	{
		/* try to scan AP */
		mrvl_scan_card(device, config);
		/* associate again */
		result = wlan_cmd_802_11_associate_cfg(card, config);
	}

	if (card->MediaConnectStatus == WlanMediaStateConnected)
		device->status = WLAN_STATUS_CONNECTED;
	else
		device->status = WLAN_STATUS_IDLE;

	/* save information on the wlan device */
	device->channel = config->channel;
	device->security = config->security;
	memcpy(device->bs_addr, config->MacAddr, sizeof(device->bs_addr));

	rt_free(config);

	if (device->status == WLAN_STATUS_CONNECTED)
	{
		eth_device_linkchange(&device->parent, RT_TRUE);
		return RT_EOK;
	}

	return -RT_ERROR;
}

static rt_err_t mrvl_wlan_disconnect(struct rt_wlan_device* device)
{
	rt_err_t result = RT_EOK;

	return RT_EOK;
}

static const struct rt_wlan_device_ops _mrvl_wlan_ops = 
{
	mrvl_wlan_probe,
	mrvl_wlan_suspend,
	mrvl_wlan_wakeup,
	mrvl_wlan_scan,
	mrvl_wlan_associate,
	mrvl_wlan_disconnect
};

rt_err_t rt_wlan_device_init(struct rt_wlan_device* device, const struct rt_wlan_device_ops* ops, char* name)
{
	rt_err_t result = RT_EOK;

	device->status = WLAN_STATUS_IDLE;
	device->channel = 1;
	device->mode = WLAN_MODE_INFRA;
	device->security = WLAN_SECURITY_OPEN;

	device->rssi = 0;

	device->ops = ops;

	rt_memset(device->ssid, 0x00, sizeof(device->ssid));
	rt_memset(device->password, 0x00, sizeof(device->password));
	rt_memset(device->bs_addr, 0x00, sizeof(device->bs_addr));

	eth_device_init((struct eth_device *)device, name);
	return result;
}

rt_err_t rt_wlan_device_control(struct rt_wlan_device* device, rt_uint8_t cmd, void* arg)
{
    rt_err_t result;

    RT_ASSERT(device != RT_NULL);

    result = RT_EOK;
    switch (cmd)
    {
    case WLAN_CTRL_SET_SSID:
        RT_ASSERT(arg != RT_NULL);

        if (rt_strlen((char*)arg) >= SSID_NAME_MAX) result = -RT_ERROR;
        else
        {
            rt_strncpy(device->ssid, (char*)arg, SSID_NAME_MAX);
        }
        break;

    case WLAN_CTRL_SET_PASSWORD:
        RT_ASSERT(arg != RT_NULL);
        
        if (rt_strlen((char*)arg) >= PASSWORD_LENGTH_MAX) result = -RT_ERROR;
        else
        {
            rt_strncpy(device->password, (char*)arg, PASSWORD_LENGTH_MAX);
        }
        break;

    case WLAN_CTRL_SET_SECURITY:
        RT_ASSERT(arg != RT_NULL);
        device->security = *(rt_uint8_t*)arg;
        break;

    case WLAN_CTRL_SET_MODE:
        RT_ASSERT(arg != RT_NULL);
        device->mode = *(rt_uint8_t*)arg;
        break;

    case WLAN_CTRL_SET_CHANNEL:
        device->channel = *(rt_uint8_t*)arg;
        break;

	case WLAN_CTRL_SET_BSADDR:
		rt_memcpy(device->bs_addr, (rt_uint8_t*) arg, MAC_LENGTH_MAX);
		break;
    }
    
    return result;
}

rt_err_t mrvl_wlan_hw_init(const char* spi_device)
{
	rt_int32_t value=0;
	rt_err_t  error=RT_EOK;

	struct rt_device* device;
	struct eth_device* eth;
	struct rt_wlan_device* wlan;

	device = (struct rt_device*)&wlan_eth;
	eth = (struct eth_device*)&wlan_eth;
	wlan = (struct rt_wlan_device*)&wlan_eth;

	/* set the RT-Thread common device interface */
	device->init  = mrvl_wlan_init;
	device->open  = mrvl_wlan_open;
	device->close = mrvl_wlan_close;
	device->read  = RT_NULL;
	device->write = mrvl_wlan_write;
	device->control	= mrvl_wlan_control;
	device->user_data =(void *)&wlan_eth ;

	/* set ethernet network interface */
	eth->eth_tx = mrvl_wlan_tx;
	eth->eth_rx = mrvl_wlan_rx;

	wlan->status = WLAN_STATUS_IDLE;

	/* try to initialize wlan card */
	if (WlanInitPhase1(&wlan_eth, spi_device) == 0)
	{
		WlanInitPhase2();
		WlanInitPhase3();

		/* initialize wlan device */
		rt_wlan_device_init(wlan, &_mrvl_wlan_ops, "w0");
	}

	return error;
}

