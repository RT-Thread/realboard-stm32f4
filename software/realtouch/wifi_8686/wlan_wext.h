/** @file wlan_wext.h
 * @brief This file contains definition for IOCTL call.
 *  
 *  Copyright © Marvell International Ltd. and/or its affiliates, 2003-2007
 */
/********************************************************
Change log:
	10/11/05: Add Doxygen format comments
	12/19/05: Correct a typo in structure _wlan_ioctl_wmm_tspec
	01/11/06: Conditionalize new scan/join ioctls
	04/10/06: Add hostcmd generic API
	04/18/06: Remove old Subscrive Event and add new Subscribe Event
	          implementation through generic hostcmd API
	06/08/06: Add definitions of custom events
	08/29/06: Add ledgpio private command
********************************************************/

#ifndef	_WLAN_WEXT_H_
#define	_WLAN_WEXT_H_
#include "wlan_defs.h"

/** wlan_ioctl_rfantenna */
typedef struct _wlan_ioctl_rfantenna
{
    u16 Action;
    u16 AntennaMode;
} wlan_ioctl_rfantenna;

/** wlan_ioctl_regrdwr */
typedef struct _wlan_ioctl_regrdwr
{
        /** Which register to access */
    u16 WhichReg;
        /** Read or Write */
    u16 Action;
    u32 Offset;
    u16 NOB;
    u32 Value;
} wlan_ioctl_regrdwr;

/** wlan_ioctl_cfregrdwr */
typedef struct _wlan_ioctl_cfregrdwr
{
        /** Read or Write */
    u8 Action;
        /** register address */
    u16 Offset;
        /** register value */
    u16 Value;
} wlan_ioctl_cfregrdwr;

/** wlan_ioctl_adhoc_key_info */
typedef struct _wlan_ioctl_adhoc_key_info
{
    u16 action;
    u8 key[16];
    u8 tkiptxmickey[16];
    u8 tkiprxmickey[16];
} wlan_ioctl_adhoc_key_info;

/** sleep_params */
typedef struct _wlan_ioctl_sleep_params_config
{
    u16 Action;
    u16 Error;
    u16 Offset;
    u16 StableTime;
    u8 CalControl;
    u8 ExtSleepClk;
    u16 Reserved;
}wlan_ioctl_sleep_params_config,
    *pwlan_ioctl_sleep_params_config;

/** BCA TIME SHARE */
typedef struct _wlan_ioctl_bca_timeshare_config
{
        /** ACT_GET/ACT_SET */
    u16 Action;
        /** Type: WLAN, BT */
    u16 TrafficType;
        /** Interval: 20msec - 60000msec */
    u32 TimeShareInterval;
        /** PTA arbiter time in msec */
    u32 BTTime;
}wlan_ioctl_bca_timeshare_config,
    *pwlan_ioctl_bca_timeshare_config;

#define MAX_CFP_LIST_NUM	64

/** wlan_ioctl_cfp_table */
typedef struct _wlan_ioctl_cfp_table
{
    u32 region;
    u32 cfp_no;
    struct
    {
        u16 Channel;
        u32 Freq;
        u16 MaxTxPower;
        BOOLEAN Unsupported;
    } cfp[MAX_CFP_LIST_NUM];
}wlan_ioctl_cfp_table, *pwlan_ioctl_cfp_table;
/** WLAN_802_11_AUTHENTICATION_MODE */
typedef enum Key_Actiond
{
    READWEPKEY= 0x00,
    SETWEPKEY= 0x01,
    NOWEPKEY= 0x3,
} WEP_Key_Actiond;

typedef struct _wep_key_set_arg{
	u8 Key_value[MRVL_NUM_WEP_KEY][16];
	u8 KeyLength[4]; /* each element key corresponds to the key_value with the same index*/
	u8 defaut_key_index; /*valide range from 0 to 3*/
}wep_key_set_Array,*Wep_Key_Set_ArrayPtr;

typedef struct _Wlan_WPA_PSK
{
    u32 KeyLength;
    u8 BSSID[6];     /*AP MAC address*/
    u8 KeyMaterial[MRVL_MAX_KEY_WPA_KEY_LENGTH];
}  WLAN_WPA_PSK_ARG,*WLAN_WPA_PSK_PTR;

#endif /* _WLAN_WEXT_H_ */
