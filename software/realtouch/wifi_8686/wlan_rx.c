/** @file wlan_rx.c
  * @brief This file contains the handling of RX in wlan
  * driver.
  * 
  *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2006
  */
/********************************************************
Change log:
	09/28/05: Add Doxygen format comments
	12/09/05: ADD Sliding window SNR/NF Average Calculation support
	
********************************************************/

#include	"include.h"

/********************************************************
		Local Variables
********************************************************/

typedef struct
{
    u8 dest_addr[6];
    u8 src_addr[6];
    u16 h803_len;
}Eth803Hdr_t;

typedef struct
{
    u8 llc_dsap;
    u8 llc_ssap;
    u8 llc_ctrl;
    u8 snap_oui[3];
    u16 snap_type;
}Rfc1042Hdr_t;

typedef struct
{
    Eth803Hdr_t eth803_hdr;
    Rfc1042Hdr_t rfc1042_hdr;
}RxPacketHdr_t;

typedef struct
{
    u8 dest_addr[6];
    u8 src_addr[6];
    u16 ethertype;
} EthII_Hdr_t;
 
/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/

/********************************************************
		Global functions
********************************************************/

/**
 *  @brief This function processes received packet and forwards it
 *  to kernel/upper layer
 *  
 *  @param priv    A pointer to wlan_private
 *  @param skb     A pointer to skb which includes the received packet
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
void ProcessRxedPacket(WlanCard *cardinfo,u8 *data,u32 len)
{
	struct pbuf* p = RT_NULL;
	RxPacketHdr_t *pRxPkt;
	RxPD *pRxPD;
	u32 pbuflen;
	int hdrChop;
	int minlen;
	EthII_Hdr_t *pEthHdr;
	Rx_Pbuf_List *RxNode = NULL;
	rt_base_t level;
	WlanCard *card = cardinfo;
	Rx_Pbuf_List *HeadNode = &card->RxList;
	const u8 rfc1042_eth_hdr[] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00 };

	RxNode = rt_malloc(sizeof(RxNode));
	if (RxNode == NULL)
	{
		WlanDebug(WlanErr,"RX packet Error: memory alloc failed\r\n");
		goto done;
	}

	pRxPD = (RxPD *) data;
	pRxPkt = (RxPacketHdr_t *) ((u8 *) pRxPD + pRxPD->PktOffset);
	/*mac source address :6byte  .mac destination address :6byte. length 2 bytes =14*/
	minlen = (8 + 4 + (pRxPD->PktOffset));
	if (len < minlen)
	{
		WlanDebug( WlanErr,"RX Error: packet length is too long\n");
		rt_free(RxNode);
		goto done;
	}

	WlanDebug(WlanData, "RX Data:%d\n",len - pRxPD->PktOffset);
	WlanDebug(WlanData, "SNR: %d, NF: %d\n", pRxPD->SNR, pRxPD->NF);
	WlanDebug(WlanData,"RX Data Dest \r\n %x,%x,%x,%x,%x,%x\r\n", pRxPkt->eth803_hdr.dest_addr[0],
			pRxPkt->eth803_hdr.dest_addr[1],
			pRxPkt->eth803_hdr.dest_addr[2],
			pRxPkt->eth803_hdr.dest_addr[3],
			pRxPkt->eth803_hdr.dest_addr[4],
			pRxPkt->eth803_hdr.dest_addr[5]);
	WlanDebug(WlanData,"RX Data Src\r\n %x,%x,%x,%x,%x,%x\r\n", pRxPkt->eth803_hdr.src_addr[0],
			pRxPkt->eth803_hdr.src_addr[1],
			pRxPkt->eth803_hdr.src_addr[2],
			pRxPkt->eth803_hdr.src_addr[3],
			pRxPkt->eth803_hdr.src_addr[4],
			pRxPkt->eth803_hdr.src_addr[5]);

	if (rt_memcmp(&pRxPkt->rfc1042_hdr, rfc1042_eth_hdr,
			sizeof(rfc1042_eth_hdr)) == 0)
	{
		/*
		 *  Replace the 803 header and rfc1042 header (llc/snap) with an
		 *    EthernetII header, keep the src/dst and snap_type (ethertype)
		 *
		 *  The firmware only passes up SNAP frames converting
		 *    all RX Data from 802.11 to 802.2/LLC/SNAP frames.
		 *
		 *  To create the Ethernet II, just move the src, dst address right
		 *    before the snap_type.
		 */
		pEthHdr = (EthII_Hdr_t *) ((u8 *) &pRxPkt->eth803_hdr
				+ sizeof(pRxPkt->eth803_hdr) + sizeof(pRxPkt->rfc1042_hdr)
				- sizeof(pRxPkt->eth803_hdr.dest_addr)
				- sizeof(pRxPkt->eth803_hdr.src_addr)
				- sizeof(pRxPkt->rfc1042_hdr.snap_type));

		rt_memcpy(pEthHdr->src_addr, pRxPkt->eth803_hdr.src_addr,
				sizeof(pEthHdr->src_addr));
		rt_memcpy(pEthHdr->dest_addr, pRxPkt->eth803_hdr.dest_addr,
				sizeof(pEthHdr->dest_addr));

		/* Chop off the RxPD + the excess memory from the 802.2/llc/snap header
		 *   that was removed
		 */
		hdrChop = (u8 *) pEthHdr - (u8 *) pRxPD;
	}
	else
	{
		hexdump("RX Data: LLC/SNAP", (u8 *) &pRxPkt->rfc1042_hdr,
				sizeof(pRxPkt->rfc1042_hdr));

		/* Chop off the RxPD */
		hdrChop = (u8 *) &pRxPkt->eth803_hdr - (u8 *) pRxPD;
		rt_free(RxNode);
	}

	/* Chop off the leading header bytes so the skb points to the start of
	 *   either the reconstructed EthII frame or the 802.2/llc/snap frame
	 */

	pbuflen = (len - hdrChop);
	if (pbuflen < 100)
		pbuflen = 100;
	p = pbuf_alloc(PBUF_LINK, pbuflen, PBUF_RAM);
	if (p == RT_NULL)
	{
		WlanDebug(WlanErr,"alloc pbuf failed length %d",pbuflen);
		rt_free(RxNode);
		return;
	}
	rt_memcpy(p->payload, (u8*) ((u32) pRxPD + hdrChop), pbuflen);
	RxNode->p = p;

	level = rt_hw_interrupt_disable();
	if (HeadNode->next == HeadNode)
	{
		HeadNode->next = RxNode;
		HeadNode->pre = RxNode;
		RxNode->next = HeadNode;
		RxNode->pre = HeadNode;
	}
	else
	{
		HeadNode->pre->next = RxNode;
		RxNode->pre = HeadNode->pre;
		HeadNode->pre = RxNode;
		RxNode->next = HeadNode;
	}
	card->RxQueueCount++;
	rt_hw_interrupt_enable(level);

done:
	return;
}
