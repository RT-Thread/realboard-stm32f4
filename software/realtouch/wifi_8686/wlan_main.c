/* @file wlan_main.c
  *
  * @brief This file contains the major functions in WLAN
  * driver. It includes init, exit, open, close and main
  * thread etc..
  *
  */
/**
  * @mainpage M-WLAN Linux Driver
  *
  * @section overview_sec Overview
  *
  * The M-WLAN is a Linux reference driver for Marvell
  * 802.11 (a/b/g) WLAN chipset.
  *
  * @section copyright_sec Copyright
  *
  * Copyright 锟�Marvell International Ltd. and/or its affiliates, 2003-2007
  *
  */
/********************************************************
Change log:
    09/30/05: Add Doxygen format comments
    12/09/05: Add TX_QUEUE support
    01/05/06: Add kernel 2.6.x support
    01/11/06: Conditionalize new scan/join functions.
    01/12/06: Add TxLockFlag for UAPSD power save mode
              and Proprietary Periodic sleep support
********************************************************/

#include "include.h"
#include "wlan_debug.h"
#include <netif/ethernetif.h>
#include <rtdef.h>
/********************************************************
        Local Variables
********************************************************/
/*NEW*/
struct rt_event  WlanRxWakeUp;

WlanCard cardinfo;
WlanInfo wlaninfo;
/**/

u32 driver_flags;
extern int wlan_cmd_mac_multicast_adr(WlanCard *cardinfo,u16 cmd_action);
extern int wlan_Control_Mac(WlanCard *cardinfo,u16 action);
extern int WlanCardProbe(WlanCard * card);
extern int wlan_download_firmware(void);
extern int wlan_init_card(WlanCard *cardinfo);
extern int wlan_bus_config(WlanCard *cardinfo);
extern int wlan_get_Mac_Address(WlanCard *cardinfo);
extern int wlan_adpat_rate_get(WlanCard *cardinfo);
extern int wlan_RF_TX_Power_Action(WlanCard *cardinfo, u16 action,u16 CurLevel);
extern int SendSpecificBSSIDScan(WlanCard *cardinfo, u8 * bssid,char *ssid,int channel);
extern int wlan_Get_WakeUp_Method(WlanCard *cardinfo,u16 action,u16 Method);
extern int set_wep_materials(WlanCard *cardinfo,Wep_Key_Set_ArrayPtr key_set_arg);
extern int wlan_set_mac_multicast_add(WlanCard *cardinfo, Multi_Addr_Struct *mac_address_arry);
extern int wlan_set_infrastructure(WlanCard *cardinfo);
extern int wlan_set_wap (WlanCard *cardinfo, unsigned char * dstbssid);
extern void wlan_set_wpa_info(WlanConfig* config,WlanInfo * wlaninfo);

struct   rt_thread 	WlanRxThread;

#define WLAN_TX_PWR_DEFAULT     		20	/* 100mW */
#define WLAN_TX_PWR_US_DEFAULT      	20  /* 100mW */
#define WLAN_TX_PWR_JP_DEFAULT      	16  /* 50mW  */
#define WLAN_TX_PWR_FR_100MW        	20  /* 100mW */
#define WLAN_TX_PWR_EMEA_DEFAULT    	20  /* 100mW */

#define WakeUpINT 						(0x01<<3)

/* Format { Channel, Frequency (MHz), MaxTxPower } */
/* Band: 'B/G', Region: USA FCC/Canada IC */
static CHANNEL_FREQ_POWER channel_freq_power_US_BG[] = {
    {1, 2412, WLAN_TX_PWR_US_DEFAULT},
    {2, 2417, WLAN_TX_PWR_US_DEFAULT},
    {3, 2422, WLAN_TX_PWR_US_DEFAULT},
    {4, 2427, WLAN_TX_PWR_US_DEFAULT},
    {5, 2432, WLAN_TX_PWR_US_DEFAULT},
    {6, 2437, WLAN_TX_PWR_US_DEFAULT},
    {7, 2442, WLAN_TX_PWR_US_DEFAULT},
    {8, 2447, WLAN_TX_PWR_US_DEFAULT},
    {9, 2452, WLAN_TX_PWR_US_DEFAULT},
    {10, 2457, WLAN_TX_PWR_US_DEFAULT},
    {11, 2462, WLAN_TX_PWR_US_DEFAULT}
};

/* Band: 'B/G', Region: Europe ETSI */
static CHANNEL_FREQ_POWER channel_freq_power_EU_BG[] = {
    {1, 2412, WLAN_TX_PWR_EMEA_DEFAULT},
    {2, 2417, WLAN_TX_PWR_EMEA_DEFAULT},
    {3, 2422, WLAN_TX_PWR_EMEA_DEFAULT},
    {4, 2427, WLAN_TX_PWR_EMEA_DEFAULT},
    {5, 2432, WLAN_TX_PWR_EMEA_DEFAULT},
    {6, 2437, WLAN_TX_PWR_EMEA_DEFAULT},
    {7, 2442, WLAN_TX_PWR_EMEA_DEFAULT},
    {8, 2447, WLAN_TX_PWR_EMEA_DEFAULT},
    {9, 2452, WLAN_TX_PWR_EMEA_DEFAULT},
    {10, 2457, WLAN_TX_PWR_EMEA_DEFAULT},
    {11, 2462, WLAN_TX_PWR_EMEA_DEFAULT},
    {12, 2467, WLAN_TX_PWR_EMEA_DEFAULT},
    {13, 2472, WLAN_TX_PWR_EMEA_DEFAULT}
};

/* Band: 'B/G', Region: Spain */
static CHANNEL_FREQ_POWER channel_freq_power_SPN_BG[] = {
    {10, 2457, WLAN_TX_PWR_DEFAULT},
    {11, 2462, WLAN_TX_PWR_DEFAULT}
};

/* Band: 'B/G', Region: France */
static CHANNEL_FREQ_POWER channel_freq_power_FR_BG[] = {
    {10, 2457, WLAN_TX_PWR_FR_100MW},
    {11, 2462, WLAN_TX_PWR_FR_100MW},
    {12, 2467, WLAN_TX_PWR_FR_100MW},
    {13, 2472, WLAN_TX_PWR_FR_100MW}
};

/* Band: 'B/G', Region: Japan */
static CHANNEL_FREQ_POWER channel_freq_power_JPN41_BG[] = {
    {1, 2412, WLAN_TX_PWR_JP_DEFAULT},
    {2, 2417, WLAN_TX_PWR_JP_DEFAULT},
    {3, 2422, WLAN_TX_PWR_JP_DEFAULT},
    {4, 2427, WLAN_TX_PWR_JP_DEFAULT},
    {5, 2432, WLAN_TX_PWR_JP_DEFAULT},
    {6, 2437, WLAN_TX_PWR_JP_DEFAULT},
    {7, 2442, WLAN_TX_PWR_JP_DEFAULT},
    {8, 2447, WLAN_TX_PWR_JP_DEFAULT},
    {9, 2452, WLAN_TX_PWR_JP_DEFAULT},
    {10, 2457, WLAN_TX_PWR_JP_DEFAULT},
    {11, 2462, WLAN_TX_PWR_JP_DEFAULT},
    {12, 2467, WLAN_TX_PWR_JP_DEFAULT},
    {13, 2472, WLAN_TX_PWR_JP_DEFAULT}
};

/* Band: 'B/G', Region: Japan */
static CHANNEL_FREQ_POWER channel_freq_power_JPN40_BG[] = {
    {14, 2484, WLAN_TX_PWR_JP_DEFAULT}
};

/********************************************************
        Global Variables
********************************************************/
/**
 * the structure for channel, frequency and power
 */
typedef struct _region_cfp_table
{
    u8 region;
    CHANNEL_FREQ_POWER *cfp_BG;
    int cfp_no_BG;
} region_cfp_table_t;

/**
 * the structure for the mapping between region and CFP
 */
region_cfp_table_t region_cfp_table[] = {
    {0x10,                      /*US FCC */
     channel_freq_power_US_BG,
     sizeof(channel_freq_power_US_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x20,                      /*CANADA IC */
     channel_freq_power_US_BG,
     sizeof(channel_freq_power_US_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x30, /*EU*/ channel_freq_power_EU_BG,
     sizeof(channel_freq_power_EU_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x31, /*SPAIN*/ channel_freq_power_SPN_BG,
     sizeof(channel_freq_power_SPN_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x32, /*FRANCE*/ channel_freq_power_FR_BG,
     sizeof(channel_freq_power_FR_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x40, /*JAPAN*/ channel_freq_power_JPN40_BG,
     sizeof(channel_freq_power_JPN40_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
    {0x41, /*JAPAN*/ channel_freq_power_JPN41_BG,
     sizeof(channel_freq_power_JPN41_BG) / sizeof(CHANNEL_FREQ_POWER),
     }
    ,
/*Add new region here */
};

/**
 * the rates supported by the card
 */
u8 WlanDataRates[WLAN_SUPPORTED_RATES] =
    { 0x02, 0x04, 0x0B, 0x16, 0x00, 0x0C, 0x12,
    0x18, 0x24, 0x30, 0x48, 0x60, 0x6C, 0x00
};

/**
 * the rates supported
 */
u8 SupportedRates[G_SUPPORTED_RATES] =
    { 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x12, 0x18, 0x24, 0x30, 0x48,
    0x60, 0x6c, 0
};
/**
 * the global variable of a pointer to wlan_private
 * structure variable
 */

u32 DSFreqList[15] = {
    0, 2412000, 2417000, 2422000, 2427000, 2432000, 2437000, 2442000,
    2447000, 2452000, 2457000, 2462000, 2467000, 2472000, 2484000
};

/**
 * the table to keep region code
 */
u16 RegionCodeToIndex[MRVDRV_MAX_REGION_CODE] =
    { 0x10, 0x20, 0x30, 0x31, 0x32, 0x40, 0x41 };

#define WLAN_STACK_SIZE	2048
ALIGN(RT_ALIGN_SIZE)
char main_thread_stack[WLAN_STACK_SIZE];
/********************************************************
        Local Functions
********************************************************/

/**
 *  @brief This function sets multicast addresses to firmware
 *
 *  @param dev     A pointer to net_device structure
 *  @return        n/a
 */
int wlan_set_multicast_list(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	int i;
	int ret = 0;
	/* Multicast */
	card->CurrentPacketFilter &= ~HostCmd_ACT_MAC_PROMISCUOUS_ENABLE;
	card->CurrentPacketFilter &= ~HostCmd_ACT_MAC_ALL_MULTICAST_ENABLE;

	for (i = 0; i < card->NumOfMulticastMACAddr; i++)
	{
		WlanDebug(WlanMsg,"Multicast address %d:"
				"%x %x %x %x %x %x\n", i,
				card->MulticastList[i][0],
				card->MulticastList[i][1],
				card->MulticastList[i][2],
				card->MulticastList[i][3],
				card->MulticastList[i][4],
				card->MulticastList[i][5]);
	}
	/* set multicast addresses to firmware */
	ret = wlan_cmd_mac_multicast_adr(card, HostCmd_ACT_GEN_SET);
	if (ret)
	{
		WlanDebug(WlanErr,"func wlan_set_multicast_list cmd:wlan_set_multicast_list failed\r\n");
		return ret;
	}
	ret = wlan_Control_Mac(card, card->CurrentPacketFilter);
	if (ret)
	{
		WlanDebug(WlanErr,"fun:wlan_set_multicast_list cmd:wlan_Control_Mac failed\r\n");
		return ret;
	}
	return ret;
}

/********************************************************
        Global Functions
********************************************************/

/**
 * @brief This function sends the rx packets to the os from the skb queue
 *
 * @param priv  A pointer to wlan_private structure
 * @return  n/a
 */
void wlan_send_rxskbQ(WlanCard * card)
{
	struct rt_wlan_dev* netdev = card->wlandev;

	if (card->RxQueueCount == 1)
	{
		eth_device_ready(&(netdev->parent.parent));
	}
}

/**
 *  @brief This function finds the CFP in
 *  region_cfp_table based on region and band parameter.
 *
 *  @param region  The region code
 *  @param band    The band
 *  @param cfp_no  A pointer to CFP number
 *  @return        A pointer to CFP
 */
CHANNEL_FREQ_POWER * wlan_get_region_cfp_table(u8 region, u8 band, int *cfp_no)
{
	int i;

	for (i = 0; i < sizeof(region_cfp_table) / sizeof(region_cfp_table_t); i++)
	{
		if (region_cfp_table[i].region == region)
		{
			{
				*cfp_no = region_cfp_table[i].cfp_no_BG;

				return region_cfp_table[i].cfp_BG;
			}
		}
	}
	return NULL;
}

/**
 *  @brief This function sets region table.
 *
 *  @param priv    A pointer to wlan_private structure
 *  @param region  The region code
 *  @param band    The band
 *  @return        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int
wlan_set_regiontable(WlanCard *cardinfo, u8 region, u8 band)
{
	WlanCard *card = cardinfo;

	CHANNEL_FREQ_POWER *cfp;
	int cfp_no;
	rt_memset((void*) &card->region_channel, 0, sizeof(card->region_channel));

	cfp = wlan_get_region_cfp_table(region, band, &cfp_no);

	if (cfp != NULL)
	{
		card->region_channel.NrCFP = cfp_no;
		card->region_channel.CFP = cfp;
	}
	else
	{
		WlanDebug(WlanErr,"wrong region code %#x in Band B-G\n", region);
		return WLAN_STATUS_FAILURE;
	}
	card->region_channel.Valid = TRUE;
	card->region_channel.Region = region;
	card->region_channel.Band = band;

	return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function handles the interrupt. it will change PS
 *  state if applicable. it will wake up main_thread to handle
 *  the interrupt event as well.
 *
 */
void wlan_interrupt(void)
{
    rt_event_send(&WlanRxWakeUp, WakeUpINT);
}

static void wlan_rx_thread(void *data)
{
	rt_uint32_t e;
	u8 ireg = 0;
	WlanCard *card = &cardinfo;

	while (1)
	{
		rt_event_recv(&WlanRxWakeUp, WakeUpINT,
				RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &e);

		WlanDebug(WlanCmd,"#\n");
		if (sbi_get_int_status(card, &ireg))
		{
			WlanDebug(WlanErr,"reading HOST_INT_STATUS_REG failed\n");
		}
		WlanDebug(WlanCmd,"card int status %x\r\n",ireg);
		card->HisRegCpy |= ireg;

		/* Command response? */
		if (card->HisRegCpy & HIS_CmdUpLdRdy)
		{
			card->HisRegCpy &= ~HIS_CmdUpLdRdy;

			wlan_process_rx_command(card);
		}

		/* Any received data? */
		if (card->HisRegCpy & HIS_RxUpLdRdy)
		{
			struct rt_wlan_dev* netdev = card->wlandev;
			card->HisRegCpy &= ~HIS_RxUpLdRdy;

			if (card->RxQueueCount == 1)
			{
				/* notify device, RxReady */
				eth_device_ready(&(netdev->parent.parent));
			}
		}

		/* Any Card Event */
		if (card->HisRegCpy & HIS_CardEvent)
		{
			card->HisRegCpy &= ~HIS_CardEvent;
			if (sbi_read_event_cause(card))
			{
				WlanDebug(WlanCmd,"main-thread: sbi_read_event_cause failed.\n");
			}
			wlan_process_event(card);
		}
	}
}

int WlanInitPhase1(struct rt_wlan_dev* wlan_device, const char* spi_device)
{
	rt_err_t result;

	rt_memset(&wlaninfo,0x00,sizeof(WlanInfo));
	rt_memset(&cardinfo,0x00,sizeof(WlanCard));

	rt_event_init(&WlanRxWakeUp, "wint", RT_IPC_FLAG_FIFO);
	rt_event_init(&(cardinfo.cmdwaitevent), "wcmd", RT_IPC_FLAG_FIFO);

	cardinfo.wlandev = wlan_device;
	wlan_device->priv=(void*)&wlaninfo;
	wlaninfo.card=&cardinfo;
	cardinfo.WlanInfo=&wlaninfo;

	result= rt_thread_init(&WlanRxThread,
			"wlan",
			wlan_rx_thread,RT_NULL,
			(rt_uint8_t*)&main_thread_stack[0],	sizeof(main_thread_stack), 
			10, 100);
	if (result == RT_EOK) rt_thread_startup(&WlanRxThread);

	result=wlan_init_card(&cardinfo);
	if(result!=0) return -1;

	result = gspihost_init(spi_device);
	if(result<0)
	{
		WlanDebug(WlanErr,"WlanInitPhase1:Wlan SPI init Failed\n");
		return -1;
	}

	wlaninfo.WlanInitStatus = WLANPhase1InitDone;
	return 0;
}

int WlanInitPhase2(void)
{
	/* scan mode */
	if(WlanCardProbe(&cardinfo)<0)
		return -1;
	wlaninfo.WlanInitStatus=WLANCareProbeDone;

	if ((wlan_download_firmware()) != WLAN_STATUS_SUCCESS)
		return -1;

	wlaninfo.WlanInitStatus=WLANPhase2InitDone;
	return 0;
}

int WlanInitPhase3(void)
{
	int ret;
	WlanCard *card=&cardinfo;

	wlan_bus_config(card);
	wlaninfo.WlanInitStatus=WLANGBusConfigDone;
	wlan_get_Mac_Address(card);
	wlaninfo.WlanInitStatus=WLANGGetMacDone;

	card->MacAction=HostCmd_ACT_MAC_RX_ON | HostCmd_ACT_MAC_TX_ON;
	wlan_Control_Mac(card,card->MacAction);
	
	wlan_Get_WakeUp_Method(card, HostCmd_ACT_GET,0);
	wlan_adpat_rate_get(card);
	wlan_RF_TX_Power_Action(card, HostCmd_ACT_GET,0);

	wlaninfo.WlanInitStatus=WLANFWDownLoadDone;

	return ret;
}

static WlanConfig wlanconfig;
int WlanScanAP(WlanConfig* config)
{
	WlanCard *card=&cardinfo;

	SendSpecificBSSIDScan(card, config->MacAddr,config->SSID,config->channel);

	wlan_set_infrastructure(card);

	return 0;
}

void WlanStartAssociation(WlanConfig* config)
{
	WlanCard *card = &cardinfo;
	WlanConfig* specialconfig = config;

	if (config->security == WEP)
	{
		wep_key_set_Array keyArray;

		rt_memset(&keyArray, 0x00, sizeof(keyArray));
		keyArray.defaut_key_index = 0;
		memcpy(&keyArray.Key_value[0][0], specialconfig->password, 16);

		keyArray.KeyLength[0] = 16;
		set_wep_materials(card, &keyArray);

		wlan_set_wap(card, specialconfig->MacAddr);
	}
	else if (config->security == WPA_PSK || config->security == WPA2_PSK)
	{
		wlan_set_wpa_info(specialconfig, &wlaninfo);
		Wlan_Association(config, &wlaninfo);
	}
	else if (config->security == NoSecurity)
	{
		set_wep_no_key(card);
		Wlan_Association(config, &wlaninfo);
	}
	else
	{
		WlanDebug(WlanErr," start association failed \r\n");
	}
	return;
}

void WlanScanProbe(void)
{
	WlanCard *card = &cardinfo;
	ScanResultItem * item;
	int ret, i;
	u8* ptr;
	ptr = rt_malloc(1024);
	if (ptr == NULL)
	{
		WlanDebug(WlanErr,"WlanScan:alloc memory failed\r\n");
		return;
	}
	memset(ptr, 0x00, 1024);
	ret = SendScanToGetAPInfo(card, ptr, 1024);
	if (ret != 0)
		WlanDebug(WlanErr,"all channels scan failed\r\n");
	else
	{
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
		}
	}
}

void WlanScanFree(void)
{
	WlanCard *card=&cardinfo;
	card->ScanResulMuxsize=0;
	if(card->ScanResultInfo!=NULL)
	rt_free(card->ScanResultInfo);
	card->ScanResultcount=0;
	return;
}

void WlanConnect(unsigned int number, char* password)
{
	WlanCard *card=&cardinfo;
	ScanResultItem * item;
	unsigned int num=number-1;
	unsigned int codelen;
	item=card->ScanResultInfo;

	if((item==NULL)||(card->ScanResultcount<number)||(card->ScanResulMuxsize==0))
	{
		WlanDebug(WlanErr,"please execute wlan scan probe operation\r\n");
		return ;
	}
	if(password==NULL)
	{
		WlanDebug(WlanErr,"input password\r\n");
		return;
	}
	codelen=rt_strlen(password);
	if(item[num].Ssid.SsidLength!=0)
	{
		rt_kprintf("select AP: %s\r\n",item[num].Ssid.Ssid);
		rt_kprintf("set password:%s,len %d",password,codelen);
	}
	else
	{
		WlanDebug(WlanErr,"count not find SSIDr\n");
		return;
	}

	memset(&wlanconfig,0x00,sizeof(wlanconfig));
	if(codelen<=64)
	memcpy(wlanconfig.password,password,codelen);
	else
	{
		WlanDebug(WlanErr,"unsupported password format\r\n");
		return;
	}
	memcpy(wlanconfig.MacAddr,item[num].MacAddress,6);
	memcpy(wlanconfig.SSID,item[num].Ssid.Ssid,item[num].Ssid.SsidLength);
	wlanconfig.channel=item[num].Channel;
	wlanconfig.security=item[num].Security;

	rt_kprintf("mac: %02x.%02x.%02x.%02x.%02x.%02x\n", wlanconfig.MacAddr[0],
			wlanconfig.MacAddr[1],
			wlanconfig.MacAddr[2],
			wlanconfig.MacAddr[3],
			wlanconfig.MacAddr[4],
			wlanconfig.MacAddr[5]);
	rt_kprintf("ssid: %s\n", wlanconfig.SSID);
	rt_kprintf("chnl: %d, security: %d\n", wlanconfig.channel, wlanconfig.security);

	WlanScanAP(&wlanconfig);
	WlanStartAssociation(&wlanconfig);
	return;
}

void wlan_set_ap_info(int channel, const rt_uint8_t *mac_addr)
{
	wlanconfig.channel = channel;
	memcpy(wlanconfig.MacAddr, mac_addr, sizeof(wlanconfig.MacAddr));
}

void wlan_set_security(int security_mode, const char* ssid, const char* passwd)
{
	/* set security */
	wlanconfig.security = security_mode;
	
	/* copy ssid */
	rt_strncpy(wlanconfig.SSID, ssid, sizeof(wlanconfig.SSID));

	if (security_mode == NoSecurity)
		return;

	/* copy password */
	rt_strncpy(wlanconfig.password, passwd, sizeof(wlanconfig.password));
}

int WlanDirectConnect()
{
	int channel = 7;
    char *ssid = "rtthread";
    char *password = "rtthread_finsh";
	char mac[6] = {0x00, 0xc0, 0x02, 0x7a, 0x17, 0xa0};

	wlan_set_ap_info(channel, mac);
	wlan_set_security(WPA_PSK, ssid, password);

	// WlanScanAP(&wlanconfig);
	// WlanStartAssociation(&wlanconfig);

	wlan_set_infrastructure(&cardinfo);
	wlan_cmd_802_11_associate_cfg(&cardinfo, &wlanconfig);

	return 0;
}

