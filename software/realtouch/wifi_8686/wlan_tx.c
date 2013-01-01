/** @file wlan_tx.c
  * @brief This file contains the handling of TX in wlan
  * driver.
  * 
  *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2006
  */
/********************************************************
Change log:
	09/28/05: Add Doxygen format comments
	12/13/05: Add Proprietary periodic sleep support
	01/05/06: Add kernel 2.6.x support	
	04/06/06: Add TSPEC, queue metrics, and MSDU expiry support
********************************************************/

#include "include.h"
#include "wlan_wext.h"
#include <netif/ethernetif.h>

#define TX_QUEUED_PACKET_LOWER_LIMIT   40
#define TX_QUEUED_PACKET_UPPER_LIMIT   50
/** Highest priority setting for a packet (uses voice AC) */
#define WMM_HIGHEST_PRIORITY  7
/********************************************************
		Local Variables
********************************************************/

/********************************************************
		Global Variables
********************************************************/
/********************************************************
		Local Functions
********************************************************/

/** 
 *  @brief This function processes a single packet and sends
 *  to IF layer
 *  
 *  @param priv    A pointer to wlan_private structure
 *  @param skb     A pointer to skb which includes TX packet
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int SendSinglePacket(WlanCard *cardinfo, struct pbuf *packet)
{
	WlanCard *card = cardinfo;
	int ret = WLAN_STATUS_SUCCESS;
	TxPD LocalTxPD;
	TxPD *pLocalTxPD = &LocalTxPD;
	u8 *ptr = card->TmpTxBuf;
	struct pbuf *phead;
	int curlen;

	if (card->MediaConnectStatus == WlanMediaStateDisconnected)
	{
		// WlanDebug(WlanMsg,"wlan did not connected, discard packet\n");
		return -1;
	}

	rt_memset(pLocalTxPD, 0, sizeof(TxPD));
	pLocalTxPD->TxPacketLength = packet->tot_len;

	/* offset of actual data */
	pLocalTxPD->TxPacketLocation = sizeof(TxPD);

	if (pLocalTxPD->TxControl == 0)
	{
		/* TxCtrl set by user or default */
		pLocalTxPD->TxControl = card->PktTxCtrl;
	}

	rt_memcpy(pLocalTxPD->TxDestAddrHigh, packet->payload, MRVDRV_ETH_ADDR_LEN);
	rt_memcpy(ptr, pLocalTxPD, sizeof(TxPD));

	ptr += sizeof(TxPD);
	phead = (struct pbuf*) packet;
	curlen = 0;
	if (phead->tot_len >= 2048)
	{
		WlanDebug(WlanErr,"tx packet size is too long\r\n");
		goto done;
	}
	while (phead != RT_NULL && curlen < phead ->tot_len)
	{
		rt_memcpy(ptr + curlen, phead->payload, phead->len);
		curlen += phead->len;
		phead = phead->next;
	}

	ret = sbi_host_to_card(card, MVMS_DAT, card->TmpTxBuf,
			packet->tot_len + sizeof(TxPD));
	if (ret)
	{
		WlanDebug(WlanErr,"SendSinglePacket Error: sbi_host_to_card failed: 0x%X\n",ret);
		goto done;
	}

	WlanDebug(WlanEncy,"Data => FW\n");

done:
	/* need to be freed in all cases */
	return ret;
}

/********************************************************
		Global functions
********************************************************/

/** 
 *  @brief This function tells firmware to send a NULL data packet.
 *  
 *  @param priv     A pointer to wlan_private structure
 *  @param flags    Trasnit Pkt Flags
 *  @return 	    n/a
 */
int SendNullPacket(WlanCard *cardinfo, struct pbuf *packet, u8 flags)
{
	WlanCard *card = cardinfo;
	TxPD txpd;
	int ret = WLAN_STATUS_SUCCESS;
	u8 *ptr = card->TmpTxBuf;

	if (card->MediaConnectStatus == WlanMediaStateDisconnected)
	{
		ret = WLAN_STATUS_FAILURE;
		WlanDebug(WlanErr,"NULL packet wlan did not connect\r\n");
		goto done;
	}

	rt_memset(&txpd, 0, sizeof(TxPD));

	txpd.TxControl = card->PktTxCtrl;
	txpd.Flags = flags;
	txpd.Priority = WMM_HIGHEST_PRIORITY;
	txpd.TxPacketLocation = sizeof(TxPD);

	rt_memcpy(ptr, &txpd, sizeof(TxPD));

	ret = sbi_host_to_card(card, MVMS_DAT, card->TmpTxBuf, sizeof(TxPD));

	if (ret != 0)
	{
		WlanDebug(WlanErr,"TX Error: SendNullPacket failed!\n");
		goto done;
	}
	WlanDebug(WlanMsg,"Null data => FW\n");

done:
	return ret;
}
