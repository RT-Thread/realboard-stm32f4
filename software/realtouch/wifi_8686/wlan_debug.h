#ifndef _WLAN_DEBUG_H
#define _WLAN_DEBUG_H

void hexdump(char *prompt, unsigned char * buf, int len);

/* Debug Level */
extern unsigned int WlanDebugLevel;

#define WlanMsg 	(0x01<<1)
#define WlanErr  	(0x01<<3)
#define WlanCmd 	(0x01<<4)
#define WlanData 	(0x01<<5)
#define WlanEncy   (0x01<<6)
#define WlanDump 	(0x01<<7)

#define WlanDebug_WlanMsg(msg...) 	do {if (WlanDebugLevel & WlanMsg) rt_kprintf(msg);} while(0)
#define WlanDebug_WlanErr(msg...)  do {if (WlanDebugLevel & WlanErr) rt_kprintf( msg);} while(0)
#define WlanDebug_WlanCmd(msg...) 	do {if (WlanDebugLevel & WlanCmd) rt_kprintf( msg);} while(0)
#define WlanDebug_WlanData(msg...)	do {if (WlanDebugLevel & WlanData) rt_kprintf( msg);} while(0)
#define WlanDebug_WlanEncy(msg...)	do {if (WlanDebugLevel & WlanEncy) rt_kprintf( msg);} while(0)
#define WlanDebug(level,msg...) 	WlanDebug_##level(msg)

#endif

