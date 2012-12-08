/** @file wlan_decl.h
 *  @brief This file contains declaration referring to
 *  functions defined in other source files
 *
 *  Copyright © Marvell International Ltd. and/or its affiliates, 2003-2007
 */
/******************************************************
Change log:
	09/29/05: add Doxygen format comments
	01/05/06: Add kernel 2.6.x support	
	01/11/06: Conditionalize new scan/join structures.
	          Move wlan_wext statics to their source file.
******************************************************/

#ifndef _WLAN_DECL_H_
#define _WLAN_DECL_H_
/** Function Prototype Declaration */

int wlan_tx_packet(WlanInfo  *wlaninfo, struct pbuf *packet);

int SendNullPacket(WlanCard *cardinfo, struct pbuf *packet,u8 flags);



int wlan_process_event(WlanCard *card);
void wlan_interrupt(void);
void HexDump(char *prompt, u8 * data, int len);

int wlan_process_rx_command(WlanCard *card);
void wlan_process_tx(WlanCard *cardinfo,struct pbuf *packet);


int wlan_set_regiontable(WlanCard *cardinfo, u8 region, u8 band);

void ProcessRxedPacket(WlanCard *cardinfo,u8 *data,u32 len);
#endif /* _WLAN_DECL_H_ */
