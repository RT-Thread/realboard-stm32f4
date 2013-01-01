/** @file wlan_join.c
 *
 *  @brief Functions implementing wlan infrastructure and adhoc join routines
 *
 *  IOCTL handlers as well as command preperation and response routines
 *   for sending adhoc start, adhoc join, and association commands
 *   to the firmware.
 *
 *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2006
 *
 *  @sa wlan_join.h
 */
/*************************************************************
Change Log:
    01/11/06: Initial revision. Match new scan code, relocate related functions
    01/19/06: Fix failure to save adhoc ssid as current after adhoc start
    03/16/06: Add a semaphore to protect reassociation thread

************************************************************/

#include "include.h"
#include "rt_wlan_dev.h"
#include "wlan_scan.h"

/**
 *  @brief This function finds out the common rates between rate1 and rate2.
 *
 * It will fill common rates in rate1 as output if found.
 *
 * NOTE: Setting the MSB of the basic rates need to be taken
 *   care, either before or after calling this function
 *
 *  @param Adapter     A pointer to wlan_adapter structure
 *  @param rate1       the buffer which keeps input and output
 *  @param rate1_size  the size of rate1 buffer
 *  @param rate2       the buffer which keeps rate2
 *  @param rate2_size  the size of rate2 buffer.
 *
 *  @return            WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static int
get_common_rates(WlanCard *cardinfo, u8 * rate1,
                 int rate1_size, u8 * rate2, int rate2_size)
{
    u8 *ptr = rate1;
    int ret = WLAN_STATUS_SUCCESS;
    u8 *tmp = NULL;
    int i, j;
    WlanCard *card=cardinfo;
	tmp = (u8*)rt_malloc(rate1_size);
	
    if (tmp == RT_NULL) {
        WlanDebug(WlanErr,"Allocate buffer for get common rates failed\n");
        return -ENOMEM;
    }

    rt_memcpy(tmp, rate1, rate1_size);
    rt_memset(rate1, 0, rate1_size);

    for (i = 0; rate2[i] && i < rate2_size; i++) {
        for (j = 0; tmp[j] && j < rate1_size; j++) {
            /* Check common rate, excluding the bit for basic rate */
            if ((rate2[i] & 0x7F) == (tmp[j] & 0x7F)) {
                *rate1++ = tmp[j];
                break;
            }
        }
    }
   hexdump("rate1 (AP) Rates", tmp, rate1_size);
   hexdump("rate2 (Card) Rates", rate2, rate2_size);
   hexdump("Common Rates", ptr, rate1 - ptr);
   WlanDebug(WlanData,"Tx DataRate is set to 0x%x\n", card->DataRate);
    if (!card->Is_DataRate_Auto) {
        while (*ptr) {
            if ((*ptr & 0x7f) == card->DataRate) {
                ret = WLAN_STATUS_SUCCESS;
                goto done;
            }
            ptr++;
        }
	 WlanDebug(WlanData,"Previously set fixed data rate 0x%x isn't "
               "compatible with the network.\n", card->DataRate);

        ret = WLAN_STATUS_FAILURE;
        goto done;
    }

    ret = WLAN_STATUS_SUCCESS;
  done:
    rt_free(tmp);
    return ret;
}

/**
 *  @brief Create the intersection of the rates supported by a target BSS and
 *         our Adapter settings for use in an assoc/join command.
 *
 *  @param Adapter       A pointer to wlan_adapter structure
 *  @param pBSSDesc      BSS Descriptor whose rates are used in the setup
 *  @param pOutRates     Output: Octet array of rates common between the BSS
 *                       and the Adapter supported rates settings
 *  @param pOutRatesSize Output: Number of rates/octets set in pOutRates
 *
 *  @return              WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 *
 */
static int	setup_rates_from_bssdesc(WlanCard *cardinfo,
                         BSSDescriptor_t * pBSSDesc,
                         u8 * pOutRates, int *pOutRatesSize)
{
	u8 *card_rates;
	int card_rates_size;
	WlanCard *card=cardinfo;

	rt_memcpy(pOutRates, pBSSDesc->SupportedRates, WLAN_SUPPORTED_RATES);

	card_rates = SupportedRates;
	card_rates_size = sizeof(SupportedRates);

	if (get_common_rates(card, pOutRates, WLAN_SUPPORTED_RATES,card_rates, card_rates_size)) 
	{
		*pOutRatesSize = 0;
		WlanDebug(WlanErr, "get_common_rates failed\n");
		return WLAN_STATUS_FAILURE;
	}

	*pOutRatesSize = WLAN_MIN(strlen(pOutRates), WLAN_SUPPORTED_RATES);

	return WLAN_STATUS_SUCCESS;
}

static int	setup_rates_from_cfg_bssdesc(WlanCard *cardinfo,
                         WlanConfig *cfg,
                         u8 * pOutRates, int *pOutRatesSize)
{
	u8 *card_rates;
	int card_rates_size;
	WlanCard *card=cardinfo;

	rt_memcpy(pOutRates, &(cfg->SupportedRates), WLAN_SUPPORTED_RATES);

	card_rates = SupportedRates;
	card_rates_size = sizeof(SupportedRates);

	if (get_common_rates(card, pOutRates, WLAN_SUPPORTED_RATES,card_rates, card_rates_size)) 
	{
		*pOutRatesSize = 0;
		WlanDebug(WlanErr, "get_common_rates failed\n");
		return WLAN_STATUS_FAILURE;
	}

	*pOutRatesSize = WLAN_MIN(strlen(pOutRates), WLAN_SUPPORTED_RATES);

	return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief Connect to the AP or Ad-hoc Network with specific bssid
 *
 * NOTE: Scan should be issued by application before this function is called
 *
 *  @param dev          A pointer to net_device structure
 *  @param info         A pointer to iw_request_info structure
 *  @param awrq         A pointer to iw_param structure
 *  @param extra        A pointer to extra data buf
 *  @return             WLAN_STATUS_SUCCESS --success, otherwise fail
 */
int wlan_set_wap (WlanCard *cardinfo, unsigned char * dstbssid)
{
    WlanCard *card=cardinfo;
    int ret = WLAN_STATUS_SUCCESS;

    u8 reqBSSID[ETH_ALEN];
	
    rt_memcpy(reqBSSID, dstbssid, ETH_ALEN);

    /* Search for index position in list for requested MAC */
    if(FindBSSID(card, reqBSSID, card->InfrastructureMode)!=TRUE)
    {
		return -ENETUNREACH;
    }

    if (card->InfrastructureMode == Wlan802_11Infrastructure) {
        ret = wlan_associate(card, card->ScanTable);
        if (ret) return ret;
    } 

    /* Check to see if we successfully connected */
    if (card->MediaConnectStatus == WlanMediaStateConnected) {
        ret = WLAN_STATUS_SUCCESS;
    } else {
        ret = -ENETDOWN;
    }
    return ret;
}

/**
 *  @brief Associated to a specific BSS discovered in a scan
 *
 *  @param priv      A pointer to wlan_private structure
 *  @param pBSSDesc  Pointer to the BSS descriptor to associate with.
 *
 *  @return          WLAN_STATUS_SUCCESS-success, otherwise fail
 */
int wlan_associate(WlanCard *cardinfo, BSSDescriptor_t * pBSSDesc)
{
    WlanCard *card=cardinfo;
    int ret;

    /* Return error if the Adapter or table entry is not marked as infra */
    if ((card->InfrastructureMode != Wlan802_11Infrastructure)
        || (pBSSDesc->InfrastructureMode != Wlan802_11Infrastructure)) {
        return -EINVAL;
    }

    ret= wlan_cmd_802_11_associate(card,pBSSDesc);

    if (ret ||card->MediaConnectStatus != WlanMediaStateConnected) {
        ret = -ENETUNREACH;
    }
    return ret;
}

/**
 *  @brief Append TSF tracking info from the scan table for the target AP
 *
 *  This function is called from the network join command prep. routine. 
 *    The TSF table TSF sent to the firmware contians two TSF values:
 *      - the TSF of the target AP from its previous beacon/probe response
 *      - the TSF timestamp of our local MAC at the time we observed the
 *        beacon/probe response.
 *
 *    The firmware uses the timestamp values to set an initial TSF value
 *      in the MAC for the new association after a reassociation attempt.
 *
 *  @param priv     A pointer to wlan_private structure
 *  @param ppBuffer A pointer to command buffer pointer
 *  @param pBSSDesc A pointer to the BSS Descriptor from the scan table of
 *                  the AP we are trying to join
 *
 *  @return         bytes added to the buffer
 */
static int
wlan_cmd_append_tsf_tlv(WlanCard *cardinfo, u8 ** ppBuffer,
                        BSSDescriptor_t * pBSSDesc)
{
    MrvlIEtypes_TsfTimestamp_t tsfTlv;
    u8 tsfVal[8];

    /* Null Checks */
    if (ppBuffer == 0)
        return 0;
    if (*ppBuffer == 0)
        return 0;

    tsfTlv.Header.Type = (TLV_TYPE_TSFTIMESTAMP);
    tsfTlv.Header.Len = (2 * sizeof(tsfVal));

     rt_memcpy(*ppBuffer, &tsfTlv, sizeof(tsfTlv.Header));
    *ppBuffer += sizeof(tsfTlv.Header);

    /* TSF timestamp from the firmware TSF when the bcn/prb rsp was received */
    //tsfVal = (pBSSDesc->networkTSF);
    rt_memcpy(tsfVal,pBSSDesc->networkTSF,sizeof(tsfVal));
   
    rt_memcpy(*ppBuffer, tsfVal, sizeof(tsfVal));
	 hexdump("assocate TSF:",(unsigned char *)*ppBuffer,8);
    *ppBuffer += sizeof(tsfVal);

    rt_memcpy(tsfVal, pBSSDesc->TimeStamp, sizeof(tsfVal));
 
    WlanDebug(WlanEncy, "ASSOC: TSF offset calc: %x - %x\n",
           *(u32*)(&tsfVal[0]), *(u32*)(&pBSSDesc->networkTSF[0]));

    rt_memcpy(*ppBuffer, tsfVal, sizeof(tsfVal));
	hexdump("assocate stmp:",(unsigned char *)*ppBuffer,8);
    *ppBuffer += sizeof(tsfVal);

    return  (sizeof(tsfTlv.Header) + (2 * sizeof(tsfVal)));
}
/**
 *  @brief This function prepares command of association.
 *
 *  @param priv      A pointer to wlan_private structure
 *  @param cmd       A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf Void cast of BSSDescriptor_t from the scan table to assoc
 *  @return          WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_associate(WlanCard *cardinfo,void *pdata_buf)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_ASSOCIATE *pAsso = NULL;
	BSSDescriptor_t *pBSSDesc;
	WLAN_802_11_RATES rates;
	int ratesSize;
	u8 *pos;
	u16 TmpCap;
	u8* tempuf;
	u32 temlen = 0;
	MrvlIEtypes_SsIdParamSet_t *pSsidTlv;
	MrvlIEtypes_PhyParamSet_t *pPhyTlv;
	MrvlIEtypes_SsParamSet_t *pSsTlv;
	MrvlIEtypes_RatesParamSet_t *pRatesTlv;
	MrvlIEtypes_AuthType_t *pAuthTlv;
	MrvlIEtypes_RsnParamSet_t *pRsnTlv;
	int ret = WLAN_STATUS_SUCCESS;
	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"alloc memory failed for association cmd\r\n");
		return WLAN_STATUS_FAILURE;

	}
	/* Set sequence number, command and INT option */
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_ASSOCIATE;
	CmdPtr->Result = 0;
	pAsso = &CmdPtr->params.associate;

	tempuf = rt_malloc(256);
	if (tempuf == RT_NULL)
	{
		WlanDebug(WlanErr,"wlan_cmd_802_11_associate fail to  get  mem\r\n ");

		goto done;
	}
	rt_memset(tempuf, 0x00, 256);

	pBSSDesc = (BSSDescriptor_t *) pdata_buf;
	pos = (u8 *) pAsso;

	rt_memcpy(pAsso->PeerStaAddr, pBSSDesc->MacAddress,
			sizeof(pAsso->PeerStaAddr));
	pos += sizeof(pAsso->PeerStaAddr);

	/* set the listen interval */
	pAsso->ListenInterval = card->ListenInterval;

	pos += sizeof(pAsso->CapInfo);
	pos += sizeof(pAsso->ListenInterval);
	pos += sizeof(pAsso->Reserved1);

	pSsidTlv = (MrvlIEtypes_SsIdParamSet_t *) tempuf;

	pSsidTlv->Header.Type = (TLV_TYPE_SSID);
	pSsidTlv->Header.Len = pBSSDesc->Ssid.SsidLength;
	rt_memcpy(pSsidTlv->SsId, pBSSDesc->Ssid.Ssid, pSsidTlv->Header.Len);
	temlen += sizeof(pSsidTlv->Header) + pSsidTlv->Header.Len;
	rt_memcpy(pos, (u8*) pSsidTlv, temlen);
	pos += sizeof(pSsidTlv->Header) + pSsidTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pPhyTlv = (MrvlIEtypes_PhyParamSet_t *) tempuf;
	pPhyTlv->Header.Type = TLV_TYPE_PHY_DS;
	pPhyTlv->Header.Len = sizeof(pPhyTlv->fh_ds.DsParamSet);
	rt_memcpy(&pPhyTlv->fh_ds.DsParamSet,
			&pBSSDesc->PhyParamSet.DsParamSet.CurrentChan,
			sizeof(pPhyTlv->fh_ds.DsParamSet));
	temlen = sizeof(pPhyTlv->Header) + pPhyTlv->Header.Len;
	rt_memcpy(pos, (u8*) pPhyTlv, temlen);
	pos += sizeof(pPhyTlv->Header) + pPhyTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pSsTlv = (MrvlIEtypes_SsParamSet_t *) tempuf;

	pSsTlv->Header.Type = (TLV_TYPE_CF);
	pSsTlv->Header.Len = sizeof(pSsTlv->cf_ibss.CfParamSet);

	temlen = sizeof(pSsTlv->Header) + pSsTlv->Header.Len;
	rt_memcpy(pos, (u8*) pSsTlv, temlen);
	pos += sizeof(pSsTlv->Header) + pSsTlv->Header.Len;

	/* Get the common rates supported between the driver and the BSS Desc */
	if (setup_rates_from_bssdesc(card, pBSSDesc, rates, &ratesSize))
	{
		ret = WLAN_STATUS_FAILURE;
		goto done;
	}

	/* Setup the Rates TLV in the association command */
	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pRatesTlv = (MrvlIEtypes_RatesParamSet_t *) tempuf;

	pRatesTlv->Header.Type = TLV_TYPE_RATES;
	pRatesTlv->Header.Len = ratesSize;
	rt_memcpy(pRatesTlv->Rates, rates, ratesSize);
	temlen = sizeof(pRatesTlv->Header) + ratesSize;
	rt_memcpy(pos, (u8*) pRatesTlv, temlen);
	pos += sizeof(pRatesTlv->Header) + ratesSize;
	/* Add the Authentication type to be used for Auth frames if needed */
	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pAuthTlv = (MrvlIEtypes_AuthType_t *) tempuf;

	pAuthTlv->Header.Type = TLV_TYPE_AUTH_TYPE;
	pAuthTlv->Header.Len = sizeof(pAuthTlv->AuthType);
	pAuthTlv->AuthType = card->SecInfo.AuthenticationMode;
	temlen = sizeof(pAuthTlv->Header) + pAuthTlv->Header.Len;
	rt_memcpy(pos, (u8*) pAuthTlv, temlen);
	pos += sizeof(pAuthTlv->Header) + pAuthTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	if (!card->wps.SessionEnable)
	{
		if (card->SecInfo.WPAEnabled || card->SecInfo.WPA2Enabled)
		{
			pRsnTlv = (MrvlIEtypes_RsnParamSet_t *) tempuf;
			pRsnTlv->Header.Type = (u16) card->Wpa_ie[0]; /* WPA_IE or WPA2_IE */
			pRsnTlv->Header.Type = pRsnTlv->Header.Type & 0x00FF;
			pRsnTlv->Header.Type = (pRsnTlv->Header.Type);
			pRsnTlv->Header.Len = (u16) card->Wpa_ie[1];
			pRsnTlv->Header.Len = pRsnTlv->Header.Len & 0x00FF;
			rt_memcpy(pRsnTlv->RsnIE, &card->Wpa_ie[2], pRsnTlv->Header.Len);
			temlen += sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len;
			rt_memcpy(pos, (u8*) pRsnTlv, temlen);
			hexdump("ASSOC_CMD: RSN IE ", (u8 *) pRsnTlv,
					sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len);
			pos += sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len;
		}
	}
	rt_free(tempuf);
	tempuf = NULL;
	// wlan_cmd_append_tsf_tlv(card, &pos, pBSSDesc);

	CmdPtr->Size = ((u16) (pos - (u8 *) pAsso) + S_DS_GEN);

	/* set the Capability info at last */
	rt_memcpy(&TmpCap, &pBSSDesc->Cap, sizeof(pAsso->CapInfo));
	TmpCap &= CAPINFO_MASK;

	TmpCap = (TmpCap);
	rt_memcpy(&pAsso->CapInfo, &TmpCap, sizeof(pAsso->CapInfo));
	ret = WlanExecuteCommand(card, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Wlan Association failure\r\n ");
		ret = WLAN_STATUS_FAILURE;
	}

done:
	if (CmdPtr != NULL)
		rt_free(CmdPtr);
	if (tempuf != NULL)
		rt_free(tempuf);

	return ret;
}

/**
 *  @brief This function prepares command of association.
 *
 *  @param priv      A pointer to wlan_private structure
 *  @param cmd       A pointer to HostCmd_DS_COMMAND structure
 *  @param pdata_buf Void cast of BSSDescriptor_t from the scan table to assoc
 *  @return          WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_associate_cfg(WlanCard *cardinfo, WlanConfig* cfg)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_ASSOCIATE *pAsso = NULL;
	WLAN_802_11_RATES rates;
	int ratesSize;
	u8 *pos;
	u16 TmpCap;
	u8* tempuf;
	u32 temlen = 0;
	MrvlIEtypes_SsIdParamSet_t *pSsidTlv;
	MrvlIEtypes_PhyParamSet_t *pPhyTlv;
	MrvlIEtypes_SsParamSet_t *pSsTlv;
	MrvlIEtypes_RatesParamSet_t *pRatesTlv;
	MrvlIEtypes_AuthType_t *pAuthTlv;
	MrvlIEtypes_RsnParamSet_t *pRsnTlv;

	int ret = WLAN_STATUS_SUCCESS;

	extern WlanInfo wlaninfo;

	wlan_wpa_init();
	rt_thread_delay(RT_TICK_PER_SECOND/10);

	/* made configuration */
	wlan_set_wpa_info(cfg, &wlaninfo);

	rt_memcpy(&(cfg->SupportedRates), SupportedRates, sizeof(cfg->SupportedRates));
	card->SecInfo.AuthenticationMode = Wlan802_11AuthModeOpen;
	card->ListenInterval = 0x0A;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"alloc memory failed for association cmd\r\n");
		return WLAN_STATUS_FAILURE;
	}

	/* Set sequence number, command and INT option */
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_ASSOCIATE;
	CmdPtr->Result = 0;
	pAsso = &CmdPtr->params.associate;

	tempuf = rt_malloc(256);
	if (tempuf == RT_NULL)
	{
		WlanDebug(WlanErr,"wlan_cmd_802_11_associate fail to  get  mem\r\n ");
		goto done;
	}
	rt_memset(tempuf, 0x00, 256);

	pos = (u8 *) pAsso;

	rt_memcpy(pAsso->PeerStaAddr, cfg->MacAddr,
			sizeof(pAsso->PeerStaAddr));
	pos += sizeof(pAsso->PeerStaAddr);

	/* set the listen interval */
	pAsso->ListenInterval = card->ListenInterval;

	pos += sizeof(pAsso->CapInfo);
	pos += sizeof(pAsso->ListenInterval);
	pos += sizeof(pAsso->Reserved1);

	pSsidTlv = (MrvlIEtypes_SsIdParamSet_t *) tempuf;

	pSsidTlv->Header.Type = (TLV_TYPE_SSID);
	pSsidTlv->Header.Len = rt_strlen(cfg->SSID);
	rt_memcpy(pSsidTlv->SsId, cfg->SSID, pSsidTlv->Header.Len);
	temlen += sizeof(pSsidTlv->Header) + pSsidTlv->Header.Len;
	rt_memcpy(pos, (u8*) pSsidTlv, temlen);
	pos += sizeof(pSsidTlv->Header) + pSsidTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pPhyTlv = (MrvlIEtypes_PhyParamSet_t *) tempuf;
	pPhyTlv->Header.Type = TLV_TYPE_PHY_DS;
	pPhyTlv->Header.Len = sizeof(pPhyTlv->fh_ds.DsParamSet);
	rt_memcpy(&pPhyTlv->fh_ds.DsParamSet,
			&cfg->channel,
			sizeof(pPhyTlv->fh_ds.DsParamSet));
	temlen = sizeof(pPhyTlv->Header) + pPhyTlv->Header.Len;
	rt_memcpy(pos, (u8*) pPhyTlv, temlen);
	pos += sizeof(pPhyTlv->Header) + pPhyTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pSsTlv = (MrvlIEtypes_SsParamSet_t *) tempuf;

	pSsTlv->Header.Type = (TLV_TYPE_CF);
	pSsTlv->Header.Len = sizeof(pSsTlv->cf_ibss.CfParamSet);

	temlen = sizeof(pSsTlv->Header) + pSsTlv->Header.Len;
	rt_memcpy(pos, (u8*) pSsTlv, temlen);
	pos += sizeof(pSsTlv->Header) + pSsTlv->Header.Len;

	/* Get the common rates supported between the driver and the BSS Desc */
	if (setup_rates_from_cfg_bssdesc(card, cfg, rates, &ratesSize))
	{
		ret = WLAN_STATUS_FAILURE;
		goto done;
	}

	/* Setup the Rates TLV in the association command */
	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pRatesTlv = (MrvlIEtypes_RatesParamSet_t *) tempuf;

	pRatesTlv->Header.Type = TLV_TYPE_RATES;
	pRatesTlv->Header.Len = ratesSize;
	rt_memcpy(pRatesTlv->Rates, rates, ratesSize);
	temlen = sizeof(pRatesTlv->Header) + ratesSize;
	rt_memcpy(pos, (u8*) pRatesTlv, temlen);
	pos += sizeof(pRatesTlv->Header) + ratesSize;
	/* Add the Authentication type to be used for Auth frames if needed */
	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	pAuthTlv = (MrvlIEtypes_AuthType_t *) tempuf;

	pAuthTlv->Header.Type = TLV_TYPE_AUTH_TYPE;
	pAuthTlv->Header.Len = sizeof(pAuthTlv->AuthType);
	pAuthTlv->AuthType = card->SecInfo.AuthenticationMode;
	temlen = sizeof(pAuthTlv->Header) + pAuthTlv->Header.Len;
	rt_memcpy(pos, (u8*) pAuthTlv, temlen);
	pos += sizeof(pAuthTlv->Header) + pAuthTlv->Header.Len;

	rt_memset(tempuf, 0x00, 256);
	temlen = 0;
	if (cfg->security == WPA_PSK || cfg->security == WPA2_PSK)
	{
		pRsnTlv = (MrvlIEtypes_RsnParamSet_t *) tempuf;
		pRsnTlv->Header.Type = (u16) card->Wpa_ie[0]; /* WPA_IE or WPA2_IE */
		pRsnTlv->Header.Type = pRsnTlv->Header.Type & 0x00FF;
		pRsnTlv->Header.Type = (pRsnTlv->Header.Type);
		pRsnTlv->Header.Len = (u16) card->Wpa_ie[1];
		pRsnTlv->Header.Len = pRsnTlv->Header.Len & 0x00FF;
		rt_memcpy(pRsnTlv->RsnIE, &card->Wpa_ie[2], pRsnTlv->Header.Len);
		temlen += sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len;
		rt_memcpy(pos, (u8*) pRsnTlv, temlen);
		hexdump("ASSOC_CMD: RSN IE ", (u8 *) pRsnTlv,
				sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len);
		pos += sizeof(pRsnTlv->Header) + pRsnTlv->Header.Len;
	}
	rt_free(tempuf);
	tempuf = NULL;

	CmdPtr->Size = ((u16) (pos - (u8 *) pAsso) + S_DS_GEN);

	/* set the Capability info at last */
	TmpCap = 0x0411;
	TmpCap &= CAPINFO_MASK;

	rt_memcpy(&pAsso->CapInfo, &TmpCap, sizeof(pAsso->CapInfo));
	if(cfg->security == NoSecurity)
    {
        CmdPtr->params.associate.CapInfo.Privacy = 0;
    }
    else
    {
        CmdPtr->params.associate.CapInfo.Privacy = 1;
    }

	ret = WlanExecuteCommand(card, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Wlan Association failure\r\n ");
		ret = WLAN_STATUS_FAILURE;
	}

done:
	if (CmdPtr != NULL)
		rt_free(CmdPtr);
	if (tempuf != NULL)
		rt_free(tempuf);

	return ret;
}

/**
 *  @brief Association firmware command response handler
 *
 *   The response buffer for the association command has the following
 *      memory layout.
 *
 *   For cases where an association response was not received (indicated
 *      by the CapInfo and AId field):
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(u16)):  Standard command response hdr   |
 *     .------------------------------------------------------------.
 *     |  CapInfo/Error Return(u16):                                |
 *     |           0xFFFF(-1): Internal error                       |
 *     |           0xFFFE(-2): Authentication unhandled message     |
 *     |           0xFFFD(-3): Authentication refused               |
 *     .------------------------------------------------------------.
 *     |  StatusCode(u16):                                          |
 *     |        If CapInfo is -1:                                   |
 *     |           (1) Internal processing failure                  |
 *     |           (2) Timeout waiting for AP response              |
 *     |                                                            |
 *     |        If CapInfo is -2:                                   |
 *     |           An authentication frame was received but was     |
 *     |           not handled by the firmware.  IEEE Status        |
 *     |           code for the failure is returned.                |
 *     |                                                            |
 *     |        If CapInfo is -3:                                   |
 *     |           An authentication frame was received and the     |
 *     |           StatusCode is the IEEE Status reported in the    |
 *     |           response.                                        |
 *     .------------------------------------------------------------.
 *     |  AId(u16): 0xFFFF                                          |
 *     .------------------------------------------------------------.
 *
 *
 *   For cases where an association response was received, the IEEE 
 *     standard association response frame is returned:
 *
 *     .------------------------------------------------------------.
 *     |  Header(4 * sizeof(u16)):  Standard command response hdr   |
 *     .------------------------------------------------------------.
 *     |  CapInfo(u16): IEEE Capability                             |
 *     .------------------------------------------------------------.
 *     |  StatusCode(u16): IEEE Status Code                         |
 *     .------------------------------------------------------------.
 *     |  AId(u16): IEEE Association ID                             |
 *     .------------------------------------------------------------.
 *     |  IEEE IEs(variable): Any received IEs comprising the       |
 *     |                      remaining portion of a received       |
 *     |                      association response frame.           |
 *     .------------------------------------------------------------.
 *
 *  For simplistic handling, the StatusCode field can be used to determine
 *    an association success (0) or failure (non-zero).
 *
 *  @param priv    A pointer to wlan_private structure
 *  @param resp    A pointer to HostCmd_DS_COMMAND
 *  @return        WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int
wlan_ret_802_11_associate(WlanCard *cardinfo, HostCmd_DS_COMMAND * resp)
{
    WlanCard *card=cardinfo;
    int ret = WLAN_STATUS_SUCCESS;
    IEEEtypes_AssocRsp_t *pAssocRsp;
    BSSDescriptor_t *pBSSDesc;
    WLAN_802_11_RATES rates;
    int ratesSize;

    pAssocRsp = (IEEEtypes_AssocRsp_t *) & resp->params;

    if (pAssocRsp->StatusCode) {
        WlanDebug(WlanErr,"ASSOC_RESP: Association Failed, status code = %d\n",
               pAssocRsp->StatusCode);

        ret = WLAN_STATUS_FAILURE;

        goto done;
    }

    /* Send a Media Connected event, according to the Spec */
    card->MediaConnectStatus = WlanMediaStateConnected;

    /* Set the attempted BSSID Index to current */
    pBSSDesc = card->ScanTable;

    WlanDebug(WlanMsg,"ASSOC_RESP: %s\n", pBSSDesc->Ssid.Ssid);

    /* Make a copy of current BSSID descriptor */
    rt_memcpy(&card->CurBssParams.BSSDescriptor,
           pBSSDesc, sizeof(BSSDescriptor_t));

    /* update CurBssParams */
    card->CurBssParams.BSSDescriptor.Channel
        = pBSSDesc->PhyParamSet.DsParamSet.CurrentChan;

    if (setup_rates_from_bssdesc(card, pBSSDesc, rates, &ratesSize)) {
        ret = WLAN_STATUS_FAILURE;
        goto done;
    }

    /* Copy the infra. association rates into Current BSS state structure */
    card->CurBssParams.NumOfRates = ratesSize;
    memcpy(&card->CurBssParams.DataRates, rates, ratesSize);

    /* Adjust the timestamps in the scan table to be relative to the newly
     *   associated AP's TSF
     */
    wlan_scan_update_tsf_timestamps(card, pBSSDesc);

    if (pBSSDesc->wmmIE.VendHdr.ElementId == WMM_IE) {
        card->CurBssParams.wmm_enabled = TRUE;
    } else {
        card->CurBssParams.wmm_enabled = FALSE;
    }
    WlanDebug(WlanEncy,"ASSOC_RESP: CurrentPacketFilter is %x\n",
           card->CurrentPacketFilter);

    if (card->SecInfo.WPAEnabled || card->SecInfo.WPA2Enabled)
        card->IsGTK_SET = FALSE;

    card->SNR[TYPE_RXPD][TYPE_AVG] = 0;
    card->NF[TYPE_RXPD][TYPE_AVG] = 0;

    memset(card->rawSNR, 0x00, sizeof(card->rawSNR));
    memset(card->rawNF, 0x00, sizeof(card->rawNF));
    card->nextSNRNF = 0;
    card->numSNRNF = 0;

done:
    return ret;
}

