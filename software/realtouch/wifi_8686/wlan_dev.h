/** @file wlan_dev.h
 *  @brief This file contains definitions and data structures specific
 *          to Marvell 802.11 NIC. It contains the Device Information
 *          structure wlan_adapter.  
 *
 *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2007
 */
/*************************************************************
Change log:
	09/26/05: add Doxygen format comments 
	01/11/06: Conditionalize new scan/join structures.
	04/18/06: Remove old Subscrive Event and add new Subscribe Event
		  implementation through generic hostcmd API
	05/08/06: Remove PermanentAddr from Adapter

 ************************************************************/

#ifndef _WLAN_DEV_H_
#define _WLAN_DEV_H_

#include "wlan_defs.h"
#include "rt_wlan_dev.h"
#include "hostcmd.h"
#include "wlan_scan.h"
#include "wlan_11d.h"
#include <netif/ethernetif.h>
#define	MAX_BSSID_PER_CHANNEL		16

/* For the extended Scan */
#define MAX_EXTENDED_SCAN_BSSID_LIST    MAX_BSSID_PER_CHANNEL (MRVDRV_MAX_CHANNEL_SIZE + 1)
typedef struct _PER_CHANNEL_BSSID_LIST_DATA
{
    u8 ucStart;
    u8 ucNumEntry;
} PER_CHANNEL_BSSID_LIST_DATA, *PPER_CHANNEL_BSSID_LIST_DATA;

typedef struct _MRV_BSSID_IE_LIST
{
    WLAN_802_11_FIXED_IEs FixedIE;
    u8 VariableIE[MRVDRV_SCAN_LIST_VAR_IE_SPACE];
} MRV_BSSID_IE_LIST, *PMRV_BSSID_IE_LIST;

#define	MAX_REGION_CHANNEL_NUM	2

/** Chan-Freq-TxPower mapping table*/
typedef struct _CHANNEL_FREQ_POWER
{
        /** Channel Number		*/
    u16 Channel;
        /** Frequency of this Channel	*/
    u32 Freq;
        /** Max allowed Tx power level	*/
    u16 MaxTxPower;
        /** TRUE:channel unsupported;  FLASE:supported*/
    BOOLEAN Unsupported;
} CHANNEL_FREQ_POWER;

/** region-band mapping table*/
typedef struct _REGION_CHANNEL
{
        /** TRUE if this entry is valid		     */
    BOOLEAN Valid;
        /** Region code for US, Japan ...	     */
    u8 Region;
        /** Band B/G/A, used for BAND_CONFIG cmd	     */
    u8 Band;
        /** Actual No. of elements in the array below */
    u8 NrCFP;
        /** chan-freq-txpower mapping table*/
    const CHANNEL_FREQ_POWER *CFP;
} REGION_CHANNEL;

typedef struct _wlan_802_11_security_t
{
    BOOLEAN WPAEnabled;
    BOOLEAN WPA2Enabled;
    WLAN_802_11_WEP_STATUS WEPStatus;
    WLAN_802_11_AUTHENTICATION_MODE AuthenticationMode;
    WLAN_802_11_ENCRYPTION_MODE EncryptionMode;
} wlan_802_11_security_t;

/** Current Basic Service Set State Structure */
typedef struct
{
    BSSDescriptor_t BSSDescriptor;

        /** band */
    u8 band;

        /** number of rates supported */
    int NumOfRates;

        /** supported rates*/
    u8 DataRates[WLAN_SUPPORTED_RATES];

        /** wmm enable? */
    u8 wmm_enabled;

        /** uapsd enable?*/
    u8 wmm_uapsd_enabled;
} CurrentBSSParams_t;

/** sleep_params */
typedef struct SleepParams
{
    u16 sp_error;
    u16 sp_offset;
    u16 sp_stabletime;
    u8 sp_calcontrol;
    u8 sp_extsleepclk;
    u16 sp_reserved;
} SleepParams;

/** sleep_period */
typedef struct SleepPeriod
{
    u16 period;
    u16 reserved;
} SleepPeriod;

/** info for debug purpose */
typedef struct _wlan_dbg
{
    u32 num_cmd_host_to_card_failure;
    u32 num_cmd_sleep_cfm_host_to_card_failure;
    u32 num_tx_host_to_card_failure;
    u32 num_event_deauth;
    u32 num_event_disassoc;
    u32 num_event_link_lost;
    u32 num_cmd_deauth;
    u32 num_cmd_assoc_success;
    u32 num_cmd_assoc_failure;
    u32 num_tx_timeout;
    u32 num_cmd_timeout;
    u16 TimeoutCmdId;
    u16 TimeoutCmdAct;
    u16 LastCmdId;
    u16 LastCmdRespId;
} wlan_dbg;
/* Data structure for WPS information */
typedef struct
{
    IEEEtypes_VendorSpecific_t wpsIe;
    BOOLEAN SessionEnable;
} wps_t;

typedef struct Rx_node{
	struct Rx_node* next;
	struct Rx_node* pre;
	struct pbuf* p;
}Rx_Pbuf_List;

typedef enum
{
	ScanIdle=0,
	ScanMultichs,
	ScanFilter
}SysScanStatus;

struct CardPrivate
{
	u8 ChipID;
	u8 irqnum;
	u8 HisRegCpy;
	u8 TxRetryCount;

	u16 SeqNum;
	u16 CmdResLen;
	int CurCmdRetCode;
	void *WlanInfo;
	HostCmd_DS_COMMAND* CurCmd;
	u8* CmdResBuf;
	struct rt_event cmdwaitevent;
	u32 EventCause;
	struct rt_wlan_dev *wlandev;
	u32 SentStatus;
	u32 fwCapInfo;

	/* Region Code */
	u16 RegionCode;
	u8 MyMacAddress[MRVDRV_ETH_ADDR_LEN];
	u8 MulticastList[MRVDRV_MAX_MULTICAST_LIST_SIZE][MRVDRV_ETH_ADDR_LEN];
	u32 NumOfMulticastMACAddr;
	REGION_CHANNEL region_channel;
	REGION_CHANNEL universal_channel;
	/** NIC Operation characteristics */
	u16 MacAction;
	u16 fwWakeupMethod;
	u32 DataRate;
	u16 HWRateDropMode;
	u16 RateBitmap;
	u16 Threshold;
	u16 FinalRate;
	u16 TxPowerLevel;
	u8 MaxTxPowerLevel;
	u8 MinTxPowerLevel;
	u32 ScanMode;
	u16 ScanProbes;
	u16 SpecificScanTime;
	u16 ActiveScanTime;
	u16 PassiveScanTime;
	BSSDescriptor_t *ScanTable;
	MRVL_WEP_KEY WepKey[MRVL_NUM_WEP_KEY];
	u16 CurrentWepKeyIndex;
	/** Encryption parameter */
	wlan_802_11_security_t SecInfo;
	/** NIC Operation characteristics */
	u16 CurrentPacketFilter;
	u32 MediaConnectStatus;
	WLAN_802_11_NETWORK_INFRASTRUCTURE InfrastructureMode;
	WLAN_802_11_FRAGMENTATION_THRESHOLD FragThsd;
	WLAN_802_11_RTS_THRESHOLD RTSThsd;
	/* Advanced Encryption Standard */
	BOOLEAN AdhocAESEnabled;
	/** current ssid/bssid related parameters*/
	CurrentBSSParams_t CurBssParams;
	/** number of association attempts for the current SSID cmd */
	u16 ListenInterval;
	BOOLEAN Is_DataRate_Auto;
	/** Encryption Key*/
	u8 Wpa_ie[64];
	u8 Wpa_ie_len;
	HostCmd_DS_802_11_KEY_MATERIAL aeskey;
	u8 TmpTxBuf[WLAN_UPLD_SIZE];
	u8 TmpRxBuf[MRVDRV_ETH_RX_PACKET_BUFFER_SIZE + 16];
	u8 *TmpRxBufDataPtr;
	u16 TmpRxBuflen;
	Rx_Pbuf_List RxList;
	u32 RxQueueCount;
	u32 PktTxCtrl;
	wps_t wps;
	BOOLEAN IsGTK_SET;
	u16 SNR[MAX_TYPE_B][MAX_TYPE_AVG];
	u16 NF[MAX_TYPE_B][MAX_TYPE_AVG];
	u8 rawSNR[DEFAULT_DATA_AVG_FACTOR];
	u8 rawNF[DEFAULT_DATA_AVG_FACTOR];
	u16 nextSNRNF;
	u16 numSNRNF;
	wlan_802_11d_state_t State11D;
	parsed_region_chan_11d_t parsed_region_chan;

	SysScanStatus ScanPurpose;
	ScanResultItem *ScanResultInfo;
	u32 ScanResulMuxsize;
	u32 ScanResultcount;
};

struct WlanPrivate
{
	u32 WlanInitStatus;
	u16 SentStatus;
	void *card;
};
typedef struct CardPrivate WlanCard;
typedef struct WlanPrivate WlanInfo;
#endif /* _WLAN_DEV_H_ */
