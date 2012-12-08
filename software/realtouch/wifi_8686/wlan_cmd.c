/** @file wlan_cmd.c
 *
 * @brief This file contains the handling of command.
 * it prepares command and sends it to firmware when
 * it is ready.
 *
 *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2007
 *
 */
/********************************************************
 Change log:
 10/04/05: Add Doxygen format comments
 01/05/06: Add kernel 2.6.x support
 01/11/06: Conditionalize new scan/join structures
 01/31/06: Add support to selectively enabe the FW Scan channel filter
 02/16/06: Clear scan in progress flag when scan command failed and dropped
 04/06/06: Add TSPEC, queue metrics, and MSDU expiry support
 04/18/06: Remove old Subscrive Event and add new Subscribe Event
 implementation through generic hostcmd API
 05/04/06: Add IBSS coalescing related new hostcmd handling
 08/29/06: Add ledgpio private command
 ********************************************************/

#include	"include.h"
#include	"wlan_defs.h"
int sbi_host_to_card(WlanCard *cardinfo, u8 type, u8 * payload, u16 nb);
int WlanExecuteCommand(WlanCard *cardinfo, HostCmd_DS_COMMAND *CmdPtr);
/********************************************************
 Local Variables
 ********************************************************/

static u16 Commands_Allowed_In_PS[] =
{ HostCmd_CMD_802_11_RSSI, HostCmd_CMD_802_11_HOST_SLEEP_CFG,
		HostCmd_CMD_802_11_WAKEUP_CONFIRM, };

/********************************************************
 Global Variables
 ********************************************************/

/********************************************************
 Local Functions
 ********************************************************/

#define WEP_40_BIT_LEN	5
#define WEP_104_BIT_LEN	13

/** 
 *  @brief This function prepares command of set_wep.
 *  
 *  @param card		A pointer to WlanCard structure
 *  @param cmd_oid   OID: ADD_WEP KEY or REMOVE_WEP KEY
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_set_wep(WlanCard *cardinfo, u32 cmd_oid)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_802_11_SET_WEP *wep = NULL;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	int ret = WLAN_STATUS_SUCCESS;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for set wep\r\n");
		return WLAN_STATUS_FAILURE;
	}
	cardinfo->SeqNum++;
	CmdPtr->SeqNum = cardinfo->SeqNum;
	CmdPtr->Result = 0;
	wep = &CmdPtr->params.wep;

	if (cmd_oid == OID_802_11_ADD_WEP)
	{
		CmdPtr->Command = HostCmd_CMD_802_11_SET_WEP;
		CmdPtr->Size = (sizeof(HostCmd_DS_802_11_SET_WEP) + S_DS_GEN);
		wep->Action = HostCmd_ACT_ADD;
		wep->KeyIndex = (card->CurrentWepKeyIndex & HostCmd_WEP_KEY_INDEX_MASK);

		switch (card->WepKey[0].KeyLength)
		{
		case WEP_40_BIT_LEN:
			wep->WEPTypeForKey1 = HostCmd_TYPE_WEP_40_BIT;
			rt_memmove(wep->WEP1, card->WepKey[0].KeyMaterial,
					card->WepKey[0].KeyLength);
			break;
		case WEP_104_BIT_LEN:
			wep->WEPTypeForKey1 = HostCmd_TYPE_WEP_104_BIT;
			rt_memmove(wep->WEP1, card->WepKey[0].KeyMaterial,
					card->WepKey[0].KeyLength);
			break;
		case 0:
			break;
		default:
			WlanDebug(WlanErr,"Set Wep key0 length %d\r\n",card->WepKey[0].KeyLength);
			ret = WLAN_STATUS_FAILURE;
			goto done;
		}

		switch (card->WepKey[1].KeyLength)
		{
		case WEP_40_BIT_LEN:
			wep->WEPTypeForKey2 = HostCmd_TYPE_WEP_40_BIT;
			rt_memmove(wep->WEP2, card->WepKey[1].KeyMaterial,
					card->WepKey[1].KeyLength);
			break;
		case WEP_104_BIT_LEN:
			wep->WEPTypeForKey2 = HostCmd_TYPE_WEP_104_BIT;
			rt_memmove(wep->WEP2, card->WepKey[1].KeyMaterial,
					card->WepKey[1].KeyLength);
			break;
		case 0:
			break;
		default:
			WlanDebug(WlanErr,"Set Wep Key1 Length = %d \n",card->WepKey[1].KeyLength);
			ret = WLAN_STATUS_FAILURE;
			goto done;
		}

		switch (card->WepKey[2].KeyLength)
		{
		case WEP_40_BIT_LEN:
			wep->WEPTypeForKey3 = HostCmd_TYPE_WEP_40_BIT;
			rt_memmove(wep->WEP3, card->WepKey[2].KeyMaterial,
					card->WepKey[2].KeyLength);
			break;
		case WEP_104_BIT_LEN:
			wep->WEPTypeForKey3 = HostCmd_TYPE_WEP_104_BIT;
			rt_memmove(wep->WEP3, card->WepKey[2].KeyMaterial,
					card->WepKey[2].KeyLength);
			break;
		case 0:
			break;
		default:
			WlanDebug(WlanErr,"Set Wep Key2 Length = %d \n",card->WepKey[1].KeyLength);
			ret = WLAN_STATUS_FAILURE;
			goto done;
		}

		switch (card->WepKey[3].KeyLength)
		{
		case WEP_40_BIT_LEN:
			wep->WEPTypeForKey4 = HostCmd_TYPE_WEP_40_BIT;
			rt_memmove(wep->WEP4, card->WepKey[3].KeyMaterial,
					card->WepKey[3].KeyLength);
			break;
		case WEP_104_BIT_LEN:
			wep->WEPTypeForKey4 = HostCmd_TYPE_WEP_104_BIT;
			rt_memmove(wep->WEP4, card->WepKey[3].KeyMaterial,
					card->WepKey[3].KeyLength);
			break;
		case 0:
			break;
		default:
			WlanDebug(WlanErr,"Set Wep Key3 Length = %d \n",card->WepKey[1].KeyLength);
			ret = WLAN_STATUS_FAILURE;
			goto done;
		}
	}
	else if (cmd_oid == OID_802_11_REMOVE_WEP)
	{
		CmdPtr->Command = (HostCmd_CMD_802_11_SET_WEP);
		CmdPtr->Size = (sizeof(HostCmd_DS_802_11_SET_WEP)) + S_DS_GEN;
		wep->Action = HostCmd_ACT_REMOVE;

		/* default tx key index */
		wep->KeyIndex = ((u16) (card->CurrentWepKeyIndex
				& (u32) HostCmd_WEP_KEY_INDEX_MASK));
	}
	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"cmd Set Wep Key failed \n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	ret = WLAN_STATUS_SUCCESS;
	done: return ret;
}

/** 
 *  @brief This function prepares command of snmp_mib.
 *  
 *  @param priv		A pointer to wlan_private structure
 *  @param cmd	   	A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action   the action: GET or SET
 *  @param cmd_oid   	the OID of SNMP MIB
 *  @param pdata_buf	the pointer to data buffer
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_snmp_mib(WlanCard *cardinfo, int cmd_action, int cmd_oid,
		void *pdata_buf)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_802_11_SNMP_MIB *pSNMPMIB = NULL;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	u8 ucTemp;
	int ret = WLAN_STATUS_SUCCESS;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for set SNMP\r\n");
		return WLAN_STATUS_FAILURE;

	}

	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_SNMP_MIB;
	CmdPtr->Result = 0;

	pSNMPMIB = &CmdPtr->params.smib;
	CmdPtr->Size = (sizeof(HostCmd_DS_802_11_SNMP_MIB) + S_DS_GEN);

	switch (cmd_oid)
	{
	case OID_802_11_INFRASTRUCTURE_MODE:
		pSNMPMIB->QueryType = (HostCmd_ACT_GEN_SET);
		pSNMPMIB->OID = ((u16) DesiredBssType_i);
		pSNMPMIB->BufSize = (sizeof(u8));
		if (card->InfrastructureMode == Wlan802_11Infrastructure)
			ucTemp = SNMP_MIB_VALUE_INFRA;
		else
			ucTemp = SNMP_MIB_VALUE_ADHOC;

		rt_memmove(pSNMPMIB->Value, &ucTemp, sizeof(u8));

		break;

	case OID_802_11D_ENABLE:
	{
		u32 ulTemp;

		pSNMPMIB->OID = (u16) Dot11D_i;

		if (cmd_action == HostCmd_ACT_SET)
		{
			pSNMPMIB->QueryType = HostCmd_ACT_GEN_SET;
			pSNMPMIB->BufSize = sizeof(u16);
			ulTemp = *(u32 *) pdata_buf;
			*((u16*) (pSNMPMIB->Value)) = ((u16) ulTemp);
		}
		break;
	}

	case OID_802_11_FRAGMENTATION_THRESHOLD:
	{
		WLAN_802_11_FRAGMENTATION_THRESHOLD ulTemp;

		pSNMPMIB->OID = ((u16) FragThresh_i);

		if (cmd_action == HostCmd_ACT_GET)
		{
			pSNMPMIB->QueryType = (HostCmd_ACT_GEN_GET);
		}
		else if (cmd_action == HostCmd_ACT_SET)
		{
			pSNMPMIB->QueryType = (HostCmd_ACT_GEN_SET);
			pSNMPMIB->BufSize = (sizeof(u16));
			ulTemp = *((WLAN_802_11_FRAGMENTATION_THRESHOLD *) pdata_buf);
			*((u16*) (pSNMPMIB->Value)) = ((u16) ulTemp);

		}

		break;
	}

	case OID_802_11_RTS_THRESHOLD:
	{

		WLAN_802_11_RTS_THRESHOLD ulTemp;
		pSNMPMIB->OID = ((u16) RtsThresh_i);

		if (cmd_action == HostCmd_ACT_GET)
		{
			pSNMPMIB->QueryType = HostCmd_ACT_GEN_GET;
		}
		else if (cmd_action == HostCmd_ACT_SET)
		{
			pSNMPMIB->QueryType = (HostCmd_ACT_GEN_SET);
			pSNMPMIB->BufSize = (sizeof(u16));
			ulTemp = *((WLAN_802_11_RTS_THRESHOLD *) pdata_buf);
			*(u16*) (pSNMPMIB->Value) = ((u16) ulTemp);

		}
		break;
	}
	case OID_802_11_TX_RETRYCOUNT:
		pSNMPMIB->OID = ((u16) ShortRetryLim_i);

		if (cmd_action == HostCmd_ACT_GET)
		{
			pSNMPMIB->QueryType = (HostCmd_ACT_GEN_GET);
		}
		else if (cmd_action == HostCmd_ACT_SET)
		{
			pSNMPMIB->QueryType = (HostCmd_ACT_GEN_SET);
			pSNMPMIB->BufSize = (sizeof(u16));
			*((u16*) (pSNMPMIB->Value)) = ((u16) card->TxRetryCount);
		}

		break;
	default:
		break;
	}

	WlanDebug(WlanCmd,"SNMP_CMD: Action=0x%x, OID=0x%x, OIDSize=0x%x, Value=0x%x\r\n",
			pSNMPMIB->QueryType, pSNMPMIB->OID, pSNMPMIB->BufSize,
			*(u16 *) pSNMPMIB->Value);

	ret = WlanExecuteCommand(card, CmdPtr);
	if (ret)
	{
		WlanDebug(WlanErr,"Failure for set SNMP\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return WLAN_STATUS_SUCCESS;
}
/** 
 *  @brief This function prepares command of mac_multicast_adr.
 *  
 *  @param priv		A pointer to WlanCard structure
 *  @param cmd_action   the action: GET or SET
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_mac_multicast_adr(WlanCard *cardinfo, u16 cmd_action)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_MAC_MULTICAST_ADR *pMCastAdr = NULL;
	int ret = WLAN_STATUS_SUCCESS;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for set MultiCast address\r\n");
		return WLAN_STATUS_FAILURE;

	}
	/* Set sequence number, command and INT option */
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_MAC_MULTICAST_ADR;
	CmdPtr->Result = 0;

	pMCastAdr = &CmdPtr->params.madr;
	CmdPtr->Size = (sizeof(HostCmd_DS_MAC_MULTICAST_ADR) + S_DS_GEN);

	pMCastAdr->Action = (cmd_action);
	pMCastAdr->NumOfAdrs = ((u16) card->NumOfMulticastMACAddr);
	rt_memcpy(pMCastAdr->MACList, card->MulticastList,
			card->NumOfMulticastMACAddr * ETH_ALEN);
	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Failure for set MultiCast address\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief This function downloads the command to firmware.
 *  
 *  @param priv		A pointer to wlan_private structure
 *  @param CmdNode   	A pointer to CmdCtrlNode structure
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */

static int DownloadCommandToStation(WlanCard *cardinfo,
		HostCmd_DS_COMMAND * Cmd)
{
	HostCmd_DS_COMMAND *CmdPtr;
	WlanCard *card = cardinfo;
	int ret = WLAN_STATUS_SUCCESS;
	rt_uint32_t event;
	CmdPtr = (HostCmd_DS_COMMAND *) Cmd;

	if (!CmdPtr || !CmdPtr->Size)
	{
		WlanDebug(WlanErr,"Download CMD parameter error\r\n");
		ret = WLAN_STATUS_FAILURE;
		goto done;
	}

	card->CurCmd = Cmd;
	//WlanDebug(WlanCmd,"send CMD: 0x%x, len %d, Number %d\n",CmdPtr->Command, CmdPtr->Size, (CmdPtr->SeqNum));
	ret = sbi_host_to_card(card, MVMS_CMD, (u8 *) CmdPtr, CmdPtr->Size);
	card->HisRegCpy &= ~HIS_TxDnLdRdy;

	rt_event_recv(&(cardinfo->cmdwaitevent), CmdWaitQWoken,
			RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &event);
	ret = WLAN_STATUS_SUCCESS;

done:
	return ret;
}

/********************************************************
 Global Functions
 ********************************************************/
/** 
 *  @brief This function executes next command in command
 *  pending queue. It will put fimware back to PS mode
 *  if applicable.
 *  
 *  @param priv     A pointer to wlan_private structure
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int WlanExecuteCommand(WlanCard *cardinfo, HostCmd_DS_COMMAND *CmdPtr)
{
	int ret = WLAN_STATUS_SUCCESS;
	DownloadCommandToStation(cardinfo, CmdPtr);

	return ret;
}

#define RF_REG_OFFSET 0x07
#define RF_REG_VALUE  0xc8
#define SPI_BMR_DELAY_METHOD		B_BIT_2
#define	SPI_BMR_BUSMODE_16_16		0x02    //16bit address and 16bit data
extern int g_dummy_clk_reg;
extern int g_dummy_clk_ioport;
extern int g_bus_mode_reg;

int wlan_init_card(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	card->ScanMode = HostCmd_BSS_TYPE_ANY;
	card->ScanProbes = 0;
	cardinfo->State11D.Enable11D = DISABLE_11D;
	memset(&(card->parsed_region_chan), 0, sizeof(parsed_region_chan_11d_t));

	/* scan time */
	card->SpecificScanTime = MRVDRV_SPECIFIC_SCAN_CHAN_TIME;
	card->ActiveScanTime = MRVDRV_ACTIVE_SCAN_CHAN_TIME;
	card->PassiveScanTime = MRVDRV_PASSIVE_SCAN_CHAN_TIME;
	card->InfrastructureMode = Wlan802_11Infrastructure;
	card->AdhocAESEnabled = FALSE;
	card->ScanPurpose = ScanIdle;

	card->ListenInterval = MRVDRV_DEFAULT_LISTEN_INTERVAL;
	card->CurrentPacketFilter = HostCmd_ACT_MAC_RX_ON | HostCmd_ACT_MAC_TX_ON;
	card->DataRate = 0; // Initially indicate the rate as auto
	card->Is_DataRate_Auto = TRUE;
	card->PktTxCtrl = 0;
	card->MediaConnectStatus = WlanMediaStateDisconnected;
	card->SeqNum = 1;
	card->RxQueueCount = 0;
	/* initialize Rx list of card */
	card->RxList.next = card->RxList.pre = &card->RxList;

	card->ScanTable = (BSSDescriptor_t *) rt_calloc(1, sizeof(BSSDescriptor_t));
	if (card->ScanTable == RT_NULL) 
	{
		WlanDebug(WlanErr, "failed to allocate memory.\n");
		return -1;
	}

	card->CmdResBuf = rt_calloc(1, WLAN_UPLD_SIZE);
	if (card->CmdResBuf == RT_NULL)
	{
		WlanDebug(WlanErr, "failed to allocate memory.\n");
		rt_free(card->ScanTable);
		card->ScanTable = RT_NULL;
		return -1;
	}

	card->ScanResultInfo = NULL;
	card->ScanResultcount = 0;
	return 0;
}

int wlan_bus_config(WlanCard *cardinfo)
{
	int ret = WLAN_STATUS_SUCCESS;
	int dummy_clk_reg;
	
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_CMD_GSPI_BUS_CONFIG *pCmdGspiConfig = NULL;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for bus config\r\n");
		return WLAN_STATUS_FAILURE;
	}
	cardinfo->SeqNum++;
	CmdPtr->SeqNum = cardinfo->SeqNum;
	CmdPtr->Command = HostCmd_CMD_GSPI_BUS_CONFIG;
	CmdPtr->Result = 0;
	CmdPtr->Size = sizeof(HostCmd_DS_CMD_GSPI_BUS_CONFIG) + S_DS_GEN;

	g_dummy_clk_ioport = 1;
	dummy_clk_reg = 1;

	pCmdGspiConfig = (HostCmd_DS_CMD_GSPI_BUS_CONFIG *) &CmdPtr->params.gspicfg;
	pCmdGspiConfig->Action = HostCmd_ACT_GEN_SET;
	pCmdGspiConfig->BusDelayMode = SPI_BMR_BUSMODE_16_16 | SPI_BMR_DELAY_METHOD;
	pCmdGspiConfig->HostTimeDelayToReadPort = g_dummy_clk_ioport * 16;
	pCmdGspiConfig->HostTimeDelayToReadregister = dummy_clk_reg * 16;

	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Failure for bus config\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return ret;
}

int wlan_get_Mac_Address(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_GET_HW_SPEC *hwspec;
	int ret = WLAN_STATUS_SUCCESS;

	rt_memset(card->MyMacAddress, 0xff, MRVDRV_ETH_ADDR_LEN);
	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for Get Mac\r\n");
		return WLAN_STATUS_FAILURE;

	}

	/* Set sequence number, command and INT option */
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_GET_HW_SPEC;
	CmdPtr->Result = 0;
	hwspec = &CmdPtr->params.hwspec;
	CmdPtr->Size = sizeof(HostCmd_DS_GET_HW_SPEC) + S_DS_GEN;
	rt_memcpy(hwspec->PermanentAddr, card->MyMacAddress, MRVDRV_ETH_ADDR_LEN);

	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Falure for Get Mac\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return ret;
}

int wlan_Control_Mac(WlanCard *cardinfo, u16 action)
{
	u16 NewMacAction;
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_MAC_CONTROL *mac;
	int ret = WLAN_STATUS_SUCCESS;
	NewMacAction = action;
	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for Mac Control\r\n");
		return WLAN_STATUS_FAILURE;

	}
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_MAC_CONTROL;
	CmdPtr->Result = 0;
	mac = &CmdPtr->params.macctrl;
	CmdPtr->Size = (sizeof(HostCmd_DS_MAC_CONTROL) + S_DS_GEN);
	mac->Action = NewMacAction;
	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Falure for  Mac Control,action 0x%x\r\n",action);
		ret = WLAN_STATUS_FAILURE;
	}

	rt_free(CmdPtr);
	return ret;
}

int wlan_Get_WakeUp_Method(WlanCard *cardinfo, u16 action, u16 Method)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_FW_WAKEUP_METHOD *fwwm;
	int ret = WLAN_STATUS_SUCCESS;
	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for Get Wakeup Method\r\n");
		return WLAN_STATUS_FAILURE;
	}
	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_FW_WAKE_METHOD;
	CmdPtr->Result = 0;
	fwwm = &CmdPtr->params.fwwakeupmethod;
	CmdPtr->Size = (sizeof(HostCmd_DS_802_11_FW_WAKEUP_METHOD) + S_DS_GEN);
	fwwm->Action = action;

	switch (action)
	{
	case HostCmd_ACT_SET:
		fwwm->Method = Method;
		break;
	case HostCmd_ACT_GET:
	default:
		fwwm->Method = 0;
		break;
	}

	ret = WlanExecuteCommand(cardinfo, CmdPtr);
	if (ret)
	{
		WlanDebug(WlanErr,"Falure for Get Wakeup Method\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return WLAN_STATUS_SUCCESS;
}

int wlan_adpat_rate_get(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_RATE_ADAPT_RATESET * rateadapt;
	int ret = WLAN_STATUS_SUCCESS;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for Get Wlan Rate\r\n");
		return WLAN_STATUS_FAILURE;
	}

	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_RATE_ADAPT_RATESET;
	CmdPtr->Result = 0;
	rateadapt = &CmdPtr->params.rateset;
	CmdPtr->Size = (sizeof(HostCmd_DS_802_11_RATE_ADAPT_RATESET) + S_DS_GEN);
	rateadapt->Action = HostCmd_ACT_GET;
	rateadapt->HWRateDropMode = 0x00;
	rateadapt->Threshold = 0x00;
	rateadapt->FinalRate = 0x00;
	rateadapt->Bitmap = 0x00;
	card->DataRate = 0;
	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Falure for Get Wlan Rate\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return WLAN_STATUS_SUCCESS;
}

int wlan_cmd_802_11_exit_pm(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *cmd = NULL;
	int ret = WLAN_STATUS_SUCCESS;

	cmd = (HostCmd_DS_COMMAND*) rt_malloc (sizeof(HostCmd_DS_COMMAND));
	if (cmd != RT_NULL)
	{
		rt_memset(cmd, 0, sizeof(HostCmd_DS_COMMAND));

		card->SeqNum++;
		cmd->SeqNum = card->SeqNum;
		cmd->Command = wlan_cpu_to_le16(HostCmd_CMD_802_11_PS_MODE);
		cmd->Size = wlan_cpu_to_le16(sizeof(HostCmd_CMD_802_11_PS_MODE) + S_DS_GEN);

		cmd->params.psmode.Action = 0x0031; /* PM EXIT */
		cmd->params.psmode.NullPktInterval = 0x2E;
		cmd->params.psmode.MultipleDtim = 0x00;

		ret = WlanExecuteCommand(card, cmd);
		if (ret)
		{
			WlanDebug(WlanErr,"failed to send command\n");
			ret = WLAN_STATUS_FAILURE;
		}

		rt_free(cmd);
		return ret;
	}
	ret = WLAN_STATUS_FAILURE;

	return ret;
}

int wlan_pm_exit(void)
{
	extern WlanCard cardinfo;
	wlan_cmd_802_11_exit_pm(&cardinfo);
	return 0;
}

/**
 *  @brief This function prepares command of rssi.
 *
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_rssi(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *cmd = NULL;
	int ret = WLAN_STATUS_SUCCESS;

	cmd = (HostCmd_DS_COMMAND*) rt_malloc (sizeof(HostCmd_DS_COMMAND));
	if (cmd != RT_NULL)
	{
		card->SeqNum++;
		cmd->SeqNum = card->SeqNum;
		cmd->Command = wlan_cpu_to_le16(HostCmd_CMD_802_11_RSSI);
		cmd->Size = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_RSSI) + S_DS_GEN);
		cmd->params.rssi.N = wlan_cpu_to_le16(5);

		ret = WlanExecuteCommand(card, cmd);
		if (ret)
		{
			WlanDebug(WlanErr,"Failure for send RSSI\n");
			ret = WLAN_STATUS_FAILURE;
		}

		rt_free(cmd);
		return ret;
	}
	ret = WLAN_STATUS_FAILURE;

	return ret;
}

int wlan_cmd_802_11_get_log(WlanCard *cardinfo)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *cmd = NULL;
	int ret = WLAN_STATUS_SUCCESS;

	cmd = (HostCmd_DS_COMMAND*) rt_malloc (sizeof(HostCmd_DS_COMMAND));
	if (cmd != RT_NULL)
	{
		rt_memset(cmd, 0, sizeof(HostCmd_DS_COMMAND));

		card->SeqNum++;
		cmd->SeqNum = card->SeqNum;
		cmd->Command = wlan_cpu_to_le16(HostCmd_CMD_802_11_GET_LOG);
		cmd->Size = wlan_cpu_to_le16(sizeof(HostCmd_DS_802_11_GET_LOG) + S_DS_GEN);

		ret = WlanExecuteCommand(card, cmd);
		if (ret)
		{
			WlanDebug(WlanErr,"Failure for send RSSI\n");
			ret = WLAN_STATUS_FAILURE;
		}

		rt_free(cmd);
		return ret;
	}
	ret = WLAN_STATUS_FAILURE;

	return ret;
}

int wlan_RF_TX_Power_Action(WlanCard *cardinfo, u16 action, u16 CurLevel)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_RF_TX_POWER *pRTP = NULL;
	int ret = WLAN_STATUS_SUCCESS;
	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));
	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for set transcever Power\r\n");
		return WLAN_STATUS_FAILURE;
	}

	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_RF_TX_POWER;
	CmdPtr->Result = 0;
	pRTP = &CmdPtr->params.txp;
	pRTP->Action = action;
	CmdPtr->Size = ((sizeof(HostCmd_DS_802_11_RF_TX_POWER)) + S_DS_GEN);
	switch (action)
	{
	case HostCmd_ACT_GEN_GET:
		pRTP->Action = (HostCmd_ACT_GEN_GET);
		pRTP->CurrentLevel = 0;
		break;
	case HostCmd_ACT_GEN_SET:
		pRTP->Action = (HostCmd_ACT_GEN_SET);
		pRTP->CurrentLevel = CurLevel;
		break;
	}

	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Falure for Set transcever power\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);
	return WLAN_STATUS_SUCCESS;

}

/** 
 *  @brief This function prepares command of key_material.
 *  
 *  @param priv		A pointer to WlanCard structure
 *  @param cmd	   	A pointer to HostCmd_DS_COMMAND structure
 *  @param cmd_action 	the action: GET or SET
 *  @param cmd_oid	OID: ENABLE or DISABLE
 *  @param pdata_buf    A pointer to data buffer
 *  @return 	   	WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_cmd_802_11_key_material(WlanCard *cardinfo, u16 cmd_action,
		WLAN_OID cmd_oid, void *pdata_buf)
{
	WlanCard *card = cardinfo;
	HostCmd_DS_COMMAND *CmdPtr = NULL;
	HostCmd_DS_802_11_KEY_MATERIAL *pKeyMaterial;
	WLAN_802_11_KEY *pKey = (WLAN_802_11_KEY *) pdata_buf;
	u16 KeyParamSet_len;
	int ret = WLAN_STATUS_SUCCESS;

	CmdPtr = rt_malloc(sizeof(HostCmd_DS_COMMAND));

	if (CmdPtr == NULL)
	{
		WlanDebug(WlanErr,"memory allocate failed for set Key\r\n");
		return WLAN_STATUS_FAILURE;
	}

	card->SeqNum++;
	CmdPtr->SeqNum = card->SeqNum;
	CmdPtr->Command = HostCmd_CMD_802_11_KEY_MATERIAL;
	CmdPtr->Result = 0;
	pKeyMaterial = &CmdPtr->params.keymaterial;
	pKeyMaterial->Action = cmd_action;

	if (cmd_action == HostCmd_ACT_GET)
	{
		CmdPtr->Size = 2 + S_DS_GEN;
		ret = WLAN_STATUS_SUCCESS;
		goto done;
	}

	rt_memset(&pKeyMaterial->KeyParamSet, 0, sizeof(MrvlIEtype_KeyParamSet_t));

	if (pKey->KeyLength == WPA_AES_KEY_LEN)
	{

		pKeyMaterial->KeyParamSet.KeyTypeId = KEY_TYPE_ID_AES;
		if (cmd_oid == (WLAN_OID) KEY_INFO_ENABLED)
			pKeyMaterial->KeyParamSet.KeyInfo = KEY_INFO_AES_ENABLED;
		else
			pKeyMaterial->KeyParamSet.KeyInfo = !(KEY_INFO_AES_ENABLED);

		if (pKey->KeyIndex & 0x40000000)
			pKeyMaterial->KeyParamSet.KeyInfo |= KEY_INFO_AES_UNICAST;
		else
			pKeyMaterial->KeyParamSet.KeyInfo |= KEY_INFO_AES_MCAST;
	}
	else if (pKey->KeyLength == WPA_TKIP_KEY_LEN)
	{
		pKeyMaterial->KeyParamSet.KeyTypeId = KEY_TYPE_ID_TKIP;
		pKeyMaterial->KeyParamSet.KeyInfo = KEY_INFO_TKIP_ENABLED;
		if (pKey->KeyIndex & 0x40000000)
			pKeyMaterial->KeyParamSet.KeyInfo |= KEY_INFO_TKIP_UNICAST;
		else
			pKeyMaterial->KeyParamSet.KeyInfo |= KEY_INFO_TKIP_MCAST;
	}

	if (pKeyMaterial->KeyParamSet.KeyTypeId)
	{
		pKeyMaterial->KeyParamSet.Type = TLV_TYPE_KEY_MATERIAL;
		pKeyMaterial->KeyParamSet.KeyLen = pKey->KeyLength;
		rt_memcpy(pKeyMaterial->KeyParamSet.Key, pKey->KeyMaterial,
				pKey->KeyLength);
		pKeyMaterial->KeyParamSet.Length = pKey->KeyLength + 6;

#define TYPE_LEN_FIELDS_LEN (4)
#define ACTION_FIELD_LEN (2)
		KeyParamSet_len = (pKey->KeyLength + 6) + TYPE_LEN_FIELDS_LEN;
		CmdPtr->Size = KeyParamSet_len + ACTION_FIELD_LEN + S_DS_GEN;
	}

	ret = WlanExecuteCommand(cardinfo, CmdPtr);

	if (ret)
	{
		WlanDebug(WlanErr,"Falure for Wlan Set Key\r\n");
		ret = WLAN_STATUS_FAILURE;
	}
	rt_free(CmdPtr);

	ret = WLAN_STATUS_SUCCESS;

done:
	return ret;
}

