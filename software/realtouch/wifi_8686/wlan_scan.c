/** @file wlan_scan.c
 *
 *  @brief Functions implementing wlan scan IOCTL and firmware command APIs
 *
 *  IOCTL handlers as well as command preperation and response routines
 *   for sending scan commands to the firmware.
 *
 *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2007
 *
 *  @sa wlan_scan.h
 */
/********************************************************
************************************************************/
#include    "include.h"

/********************************************************
    Local Constants
********************************************************/
//! Memory needed to store a max sized Channel List TLV for a firmware scan
#define CHAN_TLV_MAX_SIZE  (sizeof(MrvlIEtypesHeader_t) + (MRVDRV_MAX_CHANNELS_PER_SCAN * sizeof(ChanScanParamSet_t)))

//! Memory needed to store a max number/size SSID TLV for a firmware scan
#define SSID_TLV_MAX_SIZE  (1 * sizeof(MrvlIEtypes_SsIdParamSet_t))

//! WPS TLV MAX size is MAX IE size plus 2 bytes for u16 MRVL TLV extension
#define WPS_TLV_MAX_SIZE   (sizeof(IEEEtypes_VendorSpecific_t) + 2)

//! Maximum memory needed for a wlan_scan_cmd_config with all TLVs at max
#define MAX_SCAN_CFG_ALLOC (sizeof(wlan_scan_cmd_config)        \
                            + sizeof(MrvlIEtypes_NumProbes_t)   \
                            + CHAN_TLV_MAX_SIZE                 \
                            + SSID_TLV_MAX_SIZE                 \
                            + WPS_TLV_MAX_SIZE)

//! The maximum number of channels the firmware can scan per command
#define MRVDRV_MAX_CHANNELS_PER_SCAN   14

/**
 * @brief Number of channels to scan per firmware scan command issuance.
 *
 *  Number restricted to prevent hitting the limit on the amount of scan data
 *  returned in a single firmware scan command.
 */
#define MRVDRV_CHANNELS_PER_SCAN_CMD   4S

/********************************************************
    Local Variables and Types
********************************************************/
int wlan_execute_scan_cmd(WlanCard *cardinfo, wlan_scan_cmd_config  *pScanCfg_in);
extern int WlanExecuteCommand(WlanCard *cardinfo,HostCmd_DS_COMMAND *CmdPtr);
/**
 * @brief Interally used to send a configured scan cmd between driver routines
 */
typedef union
{
    wlan_scan_cmd_config config;        //!< Scan configuration (variable length)
    u8 configAllocBuf[MAX_SCAN_CFG_ALLOC];      //!< Max allocated block
} wlan_scan_cmd_config_tlv;
BOOLEAN FindSSIDInList(WlanCard *cardinfo, WLAN_802_11_SSID * ssid,u8 * bssid, int mode);

/**
 *  @brief Check if a scanned network compatible with the driver settings
 *
 *   WEP     WPA     WPA2    ad-hoc  encrypt                      Network
 * enabled enabled  enabled   AES     mode   Privacy  WPA  WPA2  Compatible
 *    0       0        0       0      NONE      0            0          0     yes     No security
 *    0       1        0       0       x           1x           1          x     yes     WPA
 *    0       0        1       0       x           1x           x          1     yes     WPA2
 *    0       0        0       1      NONE      1            0           0     yes    Ad-hoc AES
 *
 *    1       0        0       0      NONE      1      0    0   yes Static WEP
 *    0       0        0       0     !=NONE     1      0    0   yes Dynamic WEP
 *
 *
 *  @param Adapter A pointer to wlan_adapter
 *  @param index   Index in ScanTable to check against current driver settings
 *  @param mode    Network mode: Infrastructure or IBSS
 *
 *  @return        Index in ScanTable, or error code if negative
 */
static BOOLEAN IsNetworkCompatible(WlanCard * cardinfo, int mode)
{
    WlanCard *card=cardinfo;
    BSSDescriptor_t *pBSSDesc;
    pBSSDesc = card->ScanTable;

    /* Don't check for compatibility if roaming */
    if ((card->MediaConnectStatus == WlanMediaStateConnected)
            && (card->InfrastructureMode == Wlan802_11Infrastructure)
            && (card->InfrastructureMode == Wlan802_11Infrastructure))
    {
        return TRUE;
    }

    if (pBSSDesc->InfrastructureMode == mode)
    {
        if (card->SecInfo.WEPStatus == Wlan802_11WEPDisabled
                && !card->SecInfo.WPAEnabled
                && !card->SecInfo.WPA2Enabled
                && pBSSDesc->wpaIE.VendHdr.ElementId != WPA_IE
                && pBSSDesc->rsnIE.IeeeHdr.ElementId != RSN_IE
                && !card->AdhocAESEnabled
                && card->SecInfo.EncryptionMode == CIPHER_NONE
                && !pBSSDesc->Privacy)
        {
            /* no security */
            return TRUE;
        }
        else if (card->SecInfo.WEPStatus == Wlan802_11WEPEnabled
                 && !card->SecInfo.WPAEnabled
                 && !card->SecInfo.WPA2Enabled
                 && !card->AdhocAESEnabled && pBSSDesc->Privacy)
        {
            /* static WEP enabled */
            return TRUE;
        }
        else if (card->SecInfo.WEPStatus == Wlan802_11WEPDisabled
                 && card->SecInfo.WPAEnabled
                 && !card->SecInfo.WPA2Enabled
                 && (pBSSDesc->wpaIE.VendHdr.ElementId == WPA_IE)
                 && !card->AdhocAESEnabled
                 /* Privacy bit may NOT be set in some APs like LinkSys WRT54G
                    && pBSSDesc->Privacy */)
        {
            /* WPA enabled */
            WlanDebug(WlanEncy,"IsNetworkCompatible() WPA: wpa_ie=%x wpa2_ie=%#x WEP=%s WPA=%s WPA2=%s EncMode=%x privacy=%#x\n",
                      pBSSDesc->wpaIE.VendHdr.ElementId,
                      pBSSDesc->rsnIE.IeeeHdr.ElementId,
                      (card->SecInfo.WEPStatus ==
                       Wlan802_11WEPEnabled) ? "e" : "d",
                      (card->SecInfo.WPAEnabled) ? "e" : "d",
                      (card->SecInfo.WPA2Enabled) ? "e" : "d",
                      card->SecInfo.EncryptionMode, pBSSDesc->Privacy);

            return TRUE;
        }
        else if (card->SecInfo.WEPStatus == Wlan802_11WEPDisabled
                 && !card->SecInfo.WPAEnabled
                 && card->SecInfo.WPA2Enabled
                 && (pBSSDesc->rsnIE.IeeeHdr.ElementId == RSN_IE)
                 && !card->AdhocAESEnabled)
            /* Privacy bit may NOT be set in some APs like LinkSys WRT54G
               && pBSSDesc->Privacy */
        {
            /* WPA2 enabled */
            WlanDebug(WlanEncy,"IsNetworkCompatible() WPA2:  wpa_ie=%x wpa2_ie=%x WEP=%s WPA=%s WPA2=%s EncMode=%x privacy=%x\n",
                      pBSSDesc->wpaIE.VendHdr.ElementId,
                      pBSSDesc->rsnIE.IeeeHdr.ElementId,
                      (card->SecInfo.WEPStatus ==Wlan802_11WEPEnabled) ? "e" : "d",
                      (card->SecInfo.WPAEnabled? "e" : "d"),
                      (card->SecInfo.WPA2Enabled? "e" : "d"),
                      card->SecInfo.EncryptionMode, pBSSDesc->Privacy);
            return TRUE;
        }
        else if (card->SecInfo.WEPStatus == Wlan802_11WEPDisabled
                 && !card->SecInfo.WPAEnabled
                 && !card->SecInfo.WPA2Enabled
                 && (pBSSDesc->wpaIE.VendHdr.ElementId != WPA_IE)
                 && (pBSSDesc->rsnIE.IeeeHdr.ElementId != RSN_IE)
                 && card->AdhocAESEnabled
                 && card->SecInfo.EncryptionMode == CIPHER_NONE
                 && pBSSDesc->Privacy)
        {
            /* Ad-hoc AES enabled */
            return TRUE;
        }
        else if (card->SecInfo.WEPStatus == Wlan802_11WEPDisabled
                 && !card->SecInfo.WPAEnabled
                 && !card->SecInfo.WPA2Enabled
                 && (pBSSDesc->wpaIE.VendHdr.ElementId != WPA_IE)
                 && (pBSSDesc->rsnIE.IeeeHdr.ElementId != RSN_IE)
                 && !card->AdhocAESEnabled
                 && card->SecInfo.EncryptionMode != CIPHER_NONE
                 && pBSSDesc->Privacy)
        {
            /* dynamic WEP enabled */
            WlanDebug(WlanEncy,"IsNetworkCompatible() dynamic WEP:"
                      "wpa_ie=%#x wpa2_ie=%#x EncMode=%#x privacy=%#x\n",
                      pBSSDesc->wpaIE.VendHdr.ElementId,
                      pBSSDesc->rsnIE.IeeeHdr.ElementId,
                      card->SecInfo.EncryptionMode, pBSSDesc->Privacy);
            return TRUE;
        }

        /* security doesn't match */
        WlanDebug(WlanErr, "IsNetworkCompatible() FAILED: wpa_ie=%#x "
                  "wpa2_ie=%#x WEP=%s WPA=%s WPA2=%s EncMode=%#x privacy=%#x\n",
                  pBSSDesc->wpaIE.VendHdr.ElementId,
                  pBSSDesc->rsnIE.IeeeHdr.ElementId,
                  (card->SecInfo.WEPStatus ==
                   Wlan802_11WEPEnabled) ? "e" : "d",
                  (card->SecInfo.WPAEnabled) ? "e" : "d",
                  (card->SecInfo.WPA2Enabled) ? "e" : "d",
                  card->SecInfo.EncryptionMode, pBSSDesc->Privacy);
        return FALSE;
    }

    /* mode doesn't match */
    return FALSE;
}
/**
 *  @brief Create a channel list for the driver to scan based on region info
 *
 *  Use the driver region/band information to construct a comprehensive list
 *    of channels to scan.  This routine is used for any scan that is not
 *    provided a specific channel list to scan.
 *
 *  @param priv          A pointer to wlan_private structure
 *  @param scanChanList  Output parameter: Resulting channel list to scan
 *  @param filteredScan  Flag indicating whether or not a BSSID or SSID filter
 *                       is being sent in the command to firmware.  Used to
 *                       increase the number of channels sent in a scan
 *                       command and to disable the firmware channel scan
 *                       filter.
 *
 *  @return              void
 */
static void wlan_scan_create_channel_list(WlanCard *cardinfo,
        ChanScanParamSet_t * scanChanList,
        BOOLEAN filteredScan,u8 newscanType)
{

    WlanCard *card=cardinfo;
    REGION_CHANNEL *scanRegion;
    const CHANNEL_FREQ_POWER *cfp;
    int rgnIdx;
    int chanIdx;
    int nextChan;
    u8 scanType;

    chanIdx = 0;

    /* Set the default scan type to the user specified type, will later
     *   be changed to passive on a per channel basis if restricted by
     *   regulatory requirements (11d or 11h)
     */
    {
        if (wlan_get_state_11d(cardinfo) == ENABLE_11D &&
                card->MediaConnectStatus != WlanMediaStateConnected)
        {
            /* Scan all the supported chan for the first scan */

            scanRegion = &card->universal_channel;

            /* clear the parsed_region_chan for the first scan */
            memset(&card->parsed_region_chan, 0x00,sizeof(card->parsed_region_chan));
        }
        else
        {

            scanRegion = &card->region_channel;
        }

        for (nextChan = 0;
                nextChan < scanRegion->NrCFP; nextChan++, chanIdx++)
        {

            cfp = scanRegion->CFP + nextChan;

            if (wlan_get_state_11d(card) == ENABLE_11D)
            {
                scanType =wlan_get_scan_type_11d(cfp->Channel,
                                                 &card->parsed_region_chan);
            }

            switch (scanRegion->Band)
            {
            case BAND_B:
            case BAND_G:
            default:
                scanChanList[chanIdx].RadioType = HostCmd_SCAN_RADIO_TYPE_BG;
                break;
            }

            if (scanType == HostCmd_SCAN_TYPE_PASSIVE)
            {
                scanChanList[chanIdx].MaxScanTime =card->PassiveScanTime;
                scanChanList[chanIdx].ChanScanMode.PassiveScan = TRUE;
            }
            else
            {
                scanChanList[chanIdx].MaxScanTime =card->ActiveScanTime;
                scanChanList[chanIdx].ChanScanMode.PassiveScan = FALSE;
            }

            scanChanList[chanIdx].ChanNumber = cfp->Channel;

            if (filteredScan)
            {
                scanChanList[chanIdx].MaxScanTime =card->SpecificScanTime;
                scanChanList[chanIdx].ChanScanMode.DisableChanFilt =TRUE;
            }
        }
    }
}

void wlan_scan_process_results(WlanCard *cardinfo)
{
    WlanCard *card=cardinfo;

    if (card->MediaConnectStatus == WlanMediaStateConnected)
    {
        card->CurBssParams.BSSDescriptor.pBeaconBuf = NULL;
        card->CurBssParams.BSSDescriptor.beaconBufSize = 0;
        card->CurBssParams.BSSDescriptor.beaconBufSizeMax = 0;


        WlanDebug(WlanMsg, "wlan_scan_process_results Found current ssid/bssid \n");
        /* Make a copy of current BSSID descriptor */
        rt_memcpy(&card->CurBssParams.BSSDescriptor,
                  card->ScanTable,
                  sizeof(card->CurBssParams.BSSDescriptor));
    }
    WlanDebug(WlanMsg,"Scan: %02x:%02x:%02x:%02x:%02x:%02x RSSI[%x], SSID[%s]\n",
              card->ScanTable->MacAddress[0],
              card->ScanTable->MacAddress[1],
              card->ScanTable->MacAddress[2],
              card->ScanTable->MacAddress[3],
              card->ScanTable->MacAddress[4],
              card->ScanTable->MacAddress[5],
              (s32) card->ScanTable->Rssi,
              card->ScanTable->Ssid.Ssid);

}

/**
 *  @brief Interpret a BSS scan response returned from the firmware
 *
 *  Parse the various fixed fields and IEs passed back for a a BSS probe
 *   response or beacon from the scan command.  Record information as needed
 *   in the scan table BSSDescriptor_t for that entry.
 *
 *  @param pBSSIDEntry  Output parameter: Pointer to the BSS Entry
 *
 *  @return             WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
static u8 beaconbuf[512];
static int InterpretBSSDescriptionWithIE(BSSDescriptor_t * pBSSEntry,u8 ** pBeaconInfo, int *bytesLeft)
{
    IEEEtypes_ElementId_e elemID;
    IEEEtypes_FhParamSet_t *pFH;
    IEEEtypes_DsParamSet_t *pDS;
    IEEEtypes_CfParamSet_t *pCF;
    IEEEtypes_IbssParamSet_t *pIbss;
    IEEEtypes_CapInfo_t *pCap;
    WLAN_802_11_FIXED_IEs fixedIE;
    u8 *pCurrentPtr;
    u8 *pRate;
    u8 elemLen;
    u16 totalIeLen;
    u8 bytesToCopy;
    u8 rateSize;
    u16 beaconSize;
    BOOLEAN foundDataRateIE;
    int bytesLeftForCurrentBeacon;
    IEEEtypes_ERPInfo_t *pERPInfo;


    IEEEtypes_VendorSpecific_t *pVendorIe;
    const u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };
    const u8 wmm_oui[4] = { 0x00, 0x50, 0xf2, 0x02 };


    IEEEtypes_CountryInfoSet_t *pcountryinfo;

    foundDataRateIE = FALSE;
    rateSize = 0;
    beaconSize = 0;

    if (*bytesLeft >= sizeof(beaconSize))
    {
        /* Extract & convert beacon size from the command buffer */
        rt_memcpy(&beaconSize, *pBeaconInfo, sizeof(beaconSize));
        *bytesLeft -= sizeof(beaconSize);
        *pBeaconInfo += sizeof(beaconSize);
    }

    if (beaconSize == 0 || beaconSize > *bytesLeft)
    {

        *pBeaconInfo += *bytesLeft;
        *bytesLeft = 0;

        return WLAN_STATUS_FAILURE;
    }

    /* Initialize the current working beacon pointer for this BSS iteration */
    pCurrentPtr = *pBeaconInfo;

    /* Advance the return beacon pointer past the current beacon */
    *pBeaconInfo += beaconSize;
    *bytesLeft -= beaconSize;

    bytesLeftForCurrentBeacon = beaconSize;

    rt_memcpy(&pBSSEntry->MacAddress[0], pCurrentPtr, MRVDRV_ETH_ADDR_LEN);
    WlanDebug(WlanEncy,"InterpretIE: AP MAC Addr-%02x:%02x:%02x:%02x:%02x:%02x\n",
              pBSSEntry->MacAddress[0], pBSSEntry->MacAddress[1],
              pBSSEntry->MacAddress[2], pBSSEntry->MacAddress[3],
              pBSSEntry->MacAddress[4], pBSSEntry->MacAddress[5]);

    pCurrentPtr += MRVDRV_ETH_ADDR_LEN;
    bytesLeftForCurrentBeacon -= MRVDRV_ETH_ADDR_LEN;

    if (bytesLeftForCurrentBeacon < 12)
    {
        WlanDebug(WlanErr,"InterpretIE: Not enough bytes left\r\n");
        return WLAN_STATUS_FAILURE;
    }

    /*
     * next 4 fields are RSSI, time stamp, beacon interval,
     *   and capability information
     */

    /* RSSI is 1 byte long */
    pBSSEntry->Rssi = ((LONG) (*pCurrentPtr));
    pCurrentPtr += 1;
    bytesLeftForCurrentBeacon -= 1;

    /*
     *  The RSSI is not part of the beacon/probe response.  After we have
     *    advanced pCurrentPtr past the RSSI field, save the remaining
     *    data for use at the application layer
     */
    rt_memcpy(beaconbuf,
              pCurrentPtr,bytesLeftForCurrentBeacon);
    pBSSEntry->pBeaconBuf = beaconbuf;
    pBSSEntry->beaconBufSize = bytesLeftForCurrentBeacon;

    /* time stamp is 8 bytes long */
    rt_memcpy(fixedIE.Timestamp, pCurrentPtr, 8);
    hexdump("Scan timestamp:",(unsigned char *)pCurrentPtr,8);
    hexdump("fixed IE timestamp:",(unsigned char *)fixedIE.Timestamp,8);
    rt_memcpy(pBSSEntry->TimeStamp, pCurrentPtr, 8);
    hexdump("fixed IE timestamp:",(unsigned char *)pBSSEntry->TimeStamp,8);
    pCurrentPtr += 8;
    bytesLeftForCurrentBeacon -= 8;

    /* beacon interval is 2 bytes long */
    rt_memcpy(&fixedIE.BeaconInterval, pCurrentPtr, 2);
    pBSSEntry->BeaconPeriod = fixedIE.BeaconInterval;
    pCurrentPtr += 2;
    bytesLeftForCurrentBeacon -= 2;

    /* capability information is 2 bytes long */
    rt_memcpy(&fixedIE.Capabilities, pCurrentPtr, 2);
    WlanDebug(WlanEncy,"InterpretIE: fixedIE.Capabilities=0x%X\n",
              fixedIE.Capabilities);
    pCap = (IEEEtypes_CapInfo_t *) & fixedIE.Capabilities;
    rt_memcpy(&pBSSEntry->Cap, pCap, sizeof(IEEEtypes_CapInfo_t));
    pCurrentPtr += 2;
    bytesLeftForCurrentBeacon -= 2;

    /* rest of the current buffer are IE's */
    WlanDebug(WlanEncy,"InterpretIE: IELength for this AP = %d\n",
              bytesLeftForCurrentBeacon);

    if (pCap->Privacy)
    {
        WlanDebug(WlanEncy,"InterpretIE: AP WEP enabled\n");
        pBSSEntry->Privacy = Wlan802_11PrivFilter8021xWEP;
    }
    else
    {
        pBSSEntry->Privacy = Wlan802_11PrivFilterAcceptAll;
    }

    if (pCap->Ibss == 1)
    {
        pBSSEntry->InfrastructureMode = Wlan802_11IBSS;
    }
    else
    {
        pBSSEntry->InfrastructureMode = Wlan802_11Infrastructure;
    }

    /* process variable IE */
    while (bytesLeftForCurrentBeacon >= 2)
    {
        elemID = (IEEEtypes_ElementId_e) (*((u8 *) pCurrentPtr));
        elemLen = *((u8 *) pCurrentPtr + 1);
        totalIeLen = elemLen + sizeof(IEEEtypes_Header_t);

        if (bytesLeftForCurrentBeacon < elemLen)
        {
            WlanDebug(WlanErr,"InterpretIE: Error in processing IE, "
                      "bytes left < IE length\n");
            bytesLeftForCurrentBeacon = 0;
            continue;
        }

        switch (elemID)
        {
        case SSID:
            pBSSEntry->Ssid.SsidLength = elemLen;
            rt_memcpy(pBSSEntry->Ssid.Ssid, (pCurrentPtr + 2), elemLen);
            WlanDebug(WlanEncy,"InterpretIE: Ssid: %-32s\n", pBSSEntry->Ssid.Ssid);
            break;

        case SUPPORTED_RATES:
            rt_memcpy(pBSSEntry->DataRates, pCurrentPtr + 2, elemLen);
            rt_memcpy(pBSSEntry->SupportedRates, pCurrentPtr + 2, elemLen);

            rateSize = elemLen;
            foundDataRateIE = TRUE;
            break;

        case EXTRA_IE:
            WlanDebug(WlanEncy,"InterpretIE: EXTRA_IE Found!\n");
            pBSSEntry->extra_ie = 1;
            break;

        case FH_PARAM_SET:
            pFH = (IEEEtypes_FhParamSet_t *) pCurrentPtr;
            pBSSEntry->NetworkTypeInUse = Wlan802_11FH;
            rt_memcpy(&pBSSEntry->PhyParamSet.FhParamSet, pFH,
                      sizeof(IEEEtypes_FhParamSet_t));
            break;

        case DS_PARAM_SET:
            pDS = (IEEEtypes_DsParamSet_t *) pCurrentPtr;

            pBSSEntry->NetworkTypeInUse = Wlan802_11DS;
            pBSSEntry->Channel = pDS->CurrentChan;

            rt_memcpy(&pBSSEntry->PhyParamSet.DsParamSet, pDS,
                      sizeof(IEEEtypes_DsParamSet_t));
            break;

        case CF_PARAM_SET:
            pCF = (IEEEtypes_CfParamSet_t *) pCurrentPtr;
            rt_memcpy(&pBSSEntry->SsParamSet.CfParamSet, pCF,
                      sizeof(IEEEtypes_CfParamSet_t));
            break;

        case IBSS_PARAM_SET:
            pIbss = (IEEEtypes_IbssParamSet_t *) pCurrentPtr;
            pBSSEntry->ATIMWindow = (pIbss->AtimWindow);
            rt_memcpy(&pBSSEntry->SsParamSet.IbssParamSet, pIbss,
                      sizeof(IEEEtypes_IbssParamSet_t));
            break;

            /* Handle Country Info IE */
        case COUNTRY_INFO:
            pcountryinfo = (IEEEtypes_CountryInfoSet_t *) pCurrentPtr;

            if (pcountryinfo->Len < sizeof(pcountryinfo->CountryCode)
                    || pcountryinfo->Len > 254)
            {
                WlanDebug(WlanEncy,"InterpretIE: 11D- Err "
                          "CountryInfo len =%d min=%d max=254\n",
                          pcountryinfo->Len, sizeof(pcountryinfo->CountryCode));

                return WLAN_STATUS_FAILURE;
            }

            rt_memcpy(&pBSSEntry->CountryInfo,
                      pcountryinfo, pcountryinfo->Len + 2);

            break;
        case ERP_INFO:
            pERPInfo = (IEEEtypes_ERPInfo_t *) pCurrentPtr;
            pBSSEntry->ERPFlags = pERPInfo->ERPFlags;
            break;
        case EXTENDED_SUPPORTED_RATES:
            /*
             * only process extended supported rate
             * if data rate is already found.
             * data rate IE should come before
             * extended supported rate IE
             */
            if (foundDataRateIE)
            {
                if ((elemLen + rateSize) > WLAN_SUPPORTED_RATES)
                {
                    bytesToCopy = (WLAN_SUPPORTED_RATES - rateSize);
                }
                else
                {
                    bytesToCopy = elemLen;
                }

                pRate = (u8 *) pBSSEntry->DataRates;
                pRate += rateSize;
                rt_memcpy(pRate, pCurrentPtr + 2, bytesToCopy);

                pRate = (u8 *) pBSSEntry->SupportedRates;
                pRate += rateSize;
                rt_memcpy(pRate, pCurrentPtr + 2, bytesToCopy);
            }

            break;

        case VENDOR_SPECIFIC_221:
            pVendorIe = (IEEEtypes_VendorSpecific_t *) pCurrentPtr;

            if (!rt_memcmp(pVendorIe->VendHdr.Oui, wpa_oui, sizeof(wpa_oui)))
            {
                pBSSEntry->wpaIE.VendHdr.Len
                    = (WLAN_MIN(totalIeLen, sizeof(pBSSEntry->wpaIE))
                       - sizeof(IEEEtypes_Header_t));

                rt_memcpy(&pBSSEntry->wpaIE,
                          pCurrentPtr,
                          (pBSSEntry->wpaIE.VendHdr.Len
                           + sizeof(IEEEtypes_Header_t)));
            }
            else if (!rt_memcmp(pVendorIe->VendHdr.Oui,
                                wmm_oui, sizeof(wmm_oui)))
            {
                if (totalIeLen == sizeof(IEEEtypes_WmmParameter_t)
                        || totalIeLen == sizeof(IEEEtypes_WmmInfo_t))
                {

                    /* Only accept and copy the WMM IE if it matches
                     *  the size expected for the WMM Info IE or the
                     *  WMM Parameter IE.
                     */
                    rt_memcpy((u8 *) & pBSSEntry->wmmIE, pCurrentPtr,
                              totalIeLen);

                }
            }/* else if (!rt_memcmp(pVendorIe->VendHdr.Oui,
                               wps_oui, sizeof(wps_oui))) {
                rt_memcpy((u8 *) & pBSSEntry->wpsIE, pCurrentPtr, totalIeLen);

            }  */
            break;
        case RSN_IE:
            pBSSEntry->rsnIE.IeeeHdr.Len
                = (WLAN_MIN(totalIeLen, sizeof(pBSSEntry->rsnIE))
                   - sizeof(IEEEtypes_Header_t));

            rt_memcpy(&pBSSEntry->rsnIE,
                      pCurrentPtr,
                      pBSSEntry->rsnIE.IeeeHdr.Len + sizeof(IEEEtypes_Header_t));

            hexdump("RSNIE:",pCurrentPtr,pBSSEntry->rsnIE.IeeeHdr.Len + sizeof(IEEEtypes_Header_t));
            break;
        }

        pCurrentPtr += elemLen + 2;

        /* need to account for IE ID and IE Len */
        bytesLeftForCurrentBeacon -= (elemLen + 2);

    }                           /* while (bytesLeftForCurrentBeacon > 2) */

    return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief This function finds a specific compatible BSSID in the scan list
 *
 *  @param Adapter  A pointer to wlan_adapter
 *  @param bssid    BSSID to find in the scan list
 *  @param mode     Network mode: Infrastructure or IBSS
 *
 *  @return         index in BSSID list, or error return code (< 0)
 */
BOOLEAN FindBSSID(WlanCard *cardinfo, u8 * bssid, int mode)
{
    int ret = FALSE;
    WlanCard *card=cardinfo;

    if (!bssid)
        return -EFAULT;

    /* Look through the scan table for a compatible match. The ret return
     *   variable will be equal to the index in the scan table (greater
     *   than zero) if the network is compatible.  The loop will continue
     *   past a matched bssid that is not compatible in case there is an
     *   AP with multiple SSIDs assigned to the same BSSID
     */

    if (!rt_memcmp(card->ScanTable->MacAddress, bssid, ETH_ALEN))
    {
        switch (mode)
        {
        case Wlan802_11Infrastructure:
        case Wlan802_11IBSS:
            ret = IsNetworkCompatible(card, mode);
            break;
        default:
            ret = FALSE;
            break;
        }
    }

    return ret;
}

/**
 *  @brief Inspect the scan response buffer for pointers to expected TLVs
 *
 *  TLVs can be included at the end of the scan response BSS information.
 *    Parse the data in the buffer for pointers to TLVs that can potentially
 *    be passed back in the response
 *
 *  @param pTlv        Pointer to the start of the TLV buffer to parse
 *  @param tlvBufSize  Size of the TLV buffer
 *  @param pTsfTlv     Output parameter: Pointer to the TSF TLV if found
 *
 *  @return            void
 */
static void
wlan_ret_802_11_scan_get_tlv_ptrs(MrvlIEtypes_Data_t * pTlv,
                                  int tlvBufSize,
                                  MrvlIEtypes_TsfTimestamp_t ** pTsfTlv)
{
    MrvlIEtypes_Data_t *pCurrentTlv;
    int tlvBufLeft;
    u16 tlvType;
    u16 tlvLen;

    pCurrentTlv = pTlv;
    tlvBufLeft = tlvBufSize;
    *pTsfTlv = NULL;


    while (tlvBufLeft >= sizeof(MrvlIEtypesHeader_t))
    {
        tlvType =pCurrentTlv->Header.Type;
        tlvLen = pCurrentTlv->Header.Len;

        switch (tlvType)
        {
        case TLV_TYPE_TSFTIMESTAMP:
            WlanDebug(WlanEncy,"SCAN_RESP: TSF Timestamp TLV, len = %d\n", tlvLen);
            *pTsfTlv = (MrvlIEtypes_TsfTimestamp_t *) pCurrentTlv;
            break;

        default:
            WlanDebug(WlanEncy,"SCAN_RESP: Unhandled TLV = %d\n", tlvType);
            /* Give up, this seems corrupted */
            return;
        }                       /* switch */

        tlvBufLeft -= (sizeof(pTlv->Header) + tlvLen);
        pCurrentTlv = (MrvlIEtypes_Data_t *) (pCurrentTlv->Data + tlvLen);
    }                           /* while */
}

static int wlan_scan_channel_list(WlanCard *cardinfo,int maxChanPerScan,BOOLEAN filteredScan,
                                  wlan_scan_cmd_config * pScanCfgOut,
                                  MrvlIEtypes_ChanListParamSet_t * pChanTlvOut,
                                  ChanScanParamSet_t * pScanChanList)
{
    WlanCard *card=cardinfo;
    ChanScanParamSet_t *pTmpChan;
    ChanScanParamSet_t *pStartChan;
    u8 scanBand;
    int tlvIdx;
    int totalscantime;
    int ret;
    int doneEarly;

    if (pScanCfgOut == 0 || pChanTlvOut == 0 || pScanChanList == 0)
    {
        WlanDebug( WlanMsg,"Scan: Null detect: %x, %x, %x\n",pScanCfgOut, pChanTlvOut, pScanChanList);
        return WLAN_STATUS_FAILURE;
    }
    ret = WLAN_STATUS_SUCCESS;

    pChanTlvOut->Header.Type = TLV_TYPE_CHANLIST;

    /* Set the temp channel struct pointer to the start of the desired list */
    pTmpChan = pScanChanList;

    /* Loop through the desired channel list, sending a new firmware scan
     *   commands for each maxChanPerScan channels (or for 1,6,11 individually
     *   if configured accordingly)
     */
    while (pTmpChan->ChanNumber)
    {
        tlvIdx = 0;
        totalscantime = 0;
        pChanTlvOut->Header.Len = 0;
        scanBand = pTmpChan->RadioType;
        pStartChan = pTmpChan;
        doneEarly = FALSE;

        /* Construct the Channel TLV for the scan command.  Continue to
         *  insert channel TLVs until:
         *    - the tlvIdx hits the maximum configured per scan command
         *    - the next channel to insert is 0 (end of desired channel list)
         *    - doneEarly is set (controlling individual scanning of 1,6,11)
         */
        while (tlvIdx < maxChanPerScan && pTmpChan->ChanNumber && !doneEarly)
        {
            WlanDebug(WlanMsg,"Scan: Chan(%3d), Radio(%d), Mode(%d,%d), Dur(%d)\n",
                      pTmpChan->ChanNumber,
                      pTmpChan->RadioType,
                      pTmpChan->ChanScanMode.PassiveScan,
                      pTmpChan->ChanScanMode.DisableChanFilt,
                      pTmpChan->MaxScanTime);

            /* Copy the current channel TLV to the command being prepared */
            rt_memcpy(pChanTlvOut->ChanScanParam+ tlvIdx,pTmpChan, sizeof(pChanTlvOut->ChanScanParam));

            /* Increment the TLV header length by the size appended */
            pChanTlvOut->Header.Len += sizeof(pChanTlvOut->ChanScanParam);

            /*
             *  The tlv buffer length is set to the number of bytes of the
             *    between the channel tlv pointer and the start of the
             *    tlv buffer.  This compensates for any TLVs that were appended
             *    before the channel list.
             */
            pScanCfgOut->tlvBufferLen = ((u8 *) pChanTlvOut- pScanCfgOut->tlvBuffer);

            /*  Add the size of the channel tlv header and the data length */
            pScanCfgOut->tlvBufferLen += (sizeof(pChanTlvOut->Header)+ pChanTlvOut->Header.Len);
            tlvIdx++;

            /* Count the total scan time per command */
            totalscantime += pTmpChan->MaxScanTime;
            doneEarly = FALSE;
            /* Stop the loop if the *current* channel is in the 1,6,11 set
             *   and we are not filtering on a BSSID or SSID.
             */
            if ((!filteredScan) && ((pTmpChan->ChanNumber == 1)|| (pTmpChan->ChanNumber == 6)||( pTmpChan->ChanNumber == 11)))
            {
                doneEarly = TRUE;
            }

            /* Increment the tmp pointer to the next channel to be scanned */
            pTmpChan++;
            /* Stop the loop if the *next* channel is in the 1,6,11 set.
             *  This will cause it to be the only channel scanned on the next
             *  interation
             */
            if (!filteredScan && (pTmpChan->ChanNumber == 1
                                  || pTmpChan->ChanNumber == 6
                                  || pTmpChan->ChanNumber == 11))
            {
                doneEarly = TRUE;
            }
        }
        /* The total scan time should be less than scan command timeout value */
        if (totalscantime > MRVDRV_MAX_TOTAL_SCAN_TIME)
        {
            WlanDebug(WlanErr,"Total scan time %d ms is over limit (%d ms), scan skipped\n",totalscantime, MRVDRV_MAX_TOTAL_SCAN_TIME);
            ret = WLAN_STATUS_FAILURE;
        }
        ret = wlan_execute_scan_cmd(card, pScanCfgOut);
    }

    if (ret)
    {
        return WLAN_STATUS_FAILURE;
    }

    return WLAN_STATUS_SUCCESS;
}

static void wlan_scan_setup_scan_config(WlanCard *cardinfo,
                                        const wlan_scan_cfg * ScanCgfPutIn,
                                        wlan_scan_cmd_config * pScanCfgOut,
                                        MrvlIEtypes_ChanListParamSet_t ** ppChanTlvOut,
                                        ChanScanParamSet_t * pScanChanList,
                                        int *pMaxChanPerScan,
                                        BOOLEAN * pFilteredScan)
{
    WlanCard *card=cardinfo;
    const u8 zeroMac[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };
    MrvlIEtypes_NumProbes_t *pNumProbesTlv;
    u8 *pTlvPos;
    u16 numProbes;
    u16 ssidLen;
    int scanType;
    int scanDur;
    int channel;
    int radioType;
    int chanIdx;
    BOOLEAN ssidFilter;
    MrvlIEtypes_WildCardSsIdParamSet_t *pWildCardSsidTlv;

    pScanCfgOut->tlvBufferLen = 0;
    pTlvPos = pScanCfgOut->tlvBuffer;

    *pMaxChanPerScan = MRVDRV_MAX_CHANNELS_PER_SCAN;
    *pFilteredScan = FALSE;
    if(ScanCgfPutIn==NULL)
    {
        WlanDebug(WlanErr,"Scan argument\r\n");
        return;
    }

    pScanCfgOut->bssType = (ScanCgfPutIn->bssType ? ScanCgfPutIn->bssType :card->ScanMode);

    /* Set the number of probes to send, use Adapter setting if unset */
    numProbes = (ScanCgfPutIn->numProbes ? ScanCgfPutIn->numProbes :card->ScanProbes);

    /*
    * Set the BSSID filter to the incoming configuration,
    *   if non-zero.  If not set, it will remain disabled (all zeros).
    */
    if(rt_memcmp(ScanCgfPutIn->specificBSSID,zeroMac,ETH_ALEN)!=0)
    {
        rt_memcpy(pScanCfgOut->specificBSSID,ScanCgfPutIn->specificBSSID,
                  sizeof(pScanCfgOut->specificBSSID));
        ssidFilter=TRUE;
    }
    else
    {
        rt_memcpy(pScanCfgOut->specificBSSID,zeroMac,ETH_ALEN);
    }

    if (((*ScanCgfPutIn->ssidList.ssid|| ScanCgfPutIn->ssidList.maxLen!=0)))
    {
        ssidLen = rt_strlen(ScanCgfPutIn->ssidList.ssid) + 1;

        pWildCardSsidTlv = (MrvlIEtypes_WildCardSsIdParamSet_t *) pTlvPos;
        pWildCardSsidTlv->Header.Type= (TLV_TYPE_WILDCARDSSID);
        pWildCardSsidTlv->Header.Len= ssidLen + sizeof(pWildCardSsidTlv->MaxSsidLength);
        pWildCardSsidTlv->MaxSsidLength= ScanCgfPutIn->ssidList.maxLen;

        rt_memcpy(pWildCardSsidTlv->SsId,ScanCgfPutIn->ssidList.ssid, ssidLen);

        pTlvPos += (sizeof(pWildCardSsidTlv->Header)+ pWildCardSsidTlv->Header.Len);
        ssidFilter=TRUE;
        WlanDebug(WlanMsg,"Scan SSID filter enable %s,len %d \r\n", ScanCgfPutIn->ssidList.ssid,pWildCardSsidTlv->MaxSsidLength);
    }
    if(ssidFilter==TRUE)
        *pFilteredScan = TRUE;
    WlanDebug(WlanMsg,"Scan mode %d numProbes = %d\n", card->ScanMode,numProbes);
    /* If the input config or adapter has the number of Probes set, add tlv */
    if (numProbes)
    {

        pNumProbesTlv = (MrvlIEtypes_NumProbes_t *) pTlvPos;
        pNumProbesTlv->Header.Type = (TLV_TYPE_NUMPROBES);
        pNumProbesTlv->Header.Len = sizeof(pNumProbesTlv->NumProbes);
        pNumProbesTlv->NumProbes = (numProbes);
        pTlvPos += sizeof(pNumProbesTlv->Header) + pNumProbesTlv->Header.Len;
    }

    /*
    * Set the output for the channel TLV to the address in the tlv buffer
    *   past any TLVs that were added in this fuction (SSID, numProbes).
    *   Channel TLVs will be added past this for each scan command, preserving
    *   the TLVs that were previously added.
    */
    *ppChanTlvOut = (MrvlIEtypes_ChanListParamSet_t *) pTlvPos;

    if(ScanCgfPutIn->chanList.validflag)
    {
        channel = ScanCgfPutIn->chanList.chanNumber;
        pScanChanList ->ChanNumber = channel;

        radioType = ScanCgfPutIn->chanList.radioType;
        pScanChanList->RadioType = radioType;

        scanType = ScanCgfPutIn->chanList.scanType;

        if (scanType == HostCmd_SCAN_TYPE_PASSIVE)
        {
            pScanChanList->ChanScanMode.PassiveScan = TRUE;
        }
        else
        {
            pScanChanList->ChanScanMode.PassiveScan = FALSE;
        }

        if (ScanCgfPutIn->chanList.scanTime)
        {
            scanDur = ScanCgfPutIn->chanList.scanTime;
        }
        else
        {
            if (scanType == HostCmd_SCAN_TYPE_PASSIVE)
            {
                scanDur = card->PassiveScanTime;
            }
            else if (*pFilteredScan)
            {
                scanDur = card->SpecificScanTime;
            }
            else
            {
                scanDur = card->ActiveScanTime;
            }
        }

        pScanChanList->MinScanTime =scanDur;
        pScanChanList->MaxScanTime =(scanDur);

    }
    else
    {
        WlanDebug(WlanMsg,"Scan: Creating full  channel list\n");
        wlan_scan_create_channel_list(card, pScanChanList, *pFilteredScan,scanType);
    }
}

/**
 *  @brief Internal function used to start a scan based on an input config
 *
 *  Use the input user scan configuration information when provided in
 *    order to send the appropriate scan commands to firmware to populate or
 *    update the internal driver scan table
 *
 *  @param priv          A pointer to wlan_private structure
 *  @param ScanCgfPutIn   Pointer to the input configuration for the requested
 *                       scan.
 *
 *  @return              WLAN_STATUS_SUCCESS or < 0 if error
 */
int wlan_scan_networks(WlanCard *cardinfo, const wlan_scan_cfg * ScanCgfPutIn)
{
    WlanCard *card=cardinfo;
    MrvlIEtypes_ChanListParamSet_t *pChanTlvOut;

    ChanScanParamSet_t scanChanList[RT_WLAN_SCAN_CHAN_MAX];
    wlan_scan_cmd_config_tlv * scanCfgOut;
    BOOLEAN filteredScan;
    int maxChanPerScan;
    int ret;

    rt_memset((void*)&scanChanList, 0x00, sizeof(scanChanList));

    scanCfgOut=rt_malloc(sizeof(wlan_scan_cmd_config_tlv));
    if(scanCfgOut==NULL)
    {
        WlanDebug(WlanMsg,"Scan: failed to alloc memory size 0x%x\n",sizeof(wlan_scan_cmd_config_tlv)*RT_WLAN_SCAN_CHAN_MAX);
        return -1;
    }
    rt_memset(scanCfgOut, 0x00, sizeof(*scanCfgOut));

    wlan_scan_setup_scan_config(card,
                                ScanCgfPutIn,
                                &scanCfgOut->config,
                                &pChanTlvOut,
                                scanChanList,
                                &maxChanPerScan,
                                &filteredScan);

    ret = wlan_scan_channel_list(card,
                                 maxChanPerScan,
                                 filteredScan,
                                 &scanCfgOut->config,
                                 pChanTlvOut, scanChanList);

    /*  Process the resulting scan table:
     *    - Remove any bad ssids
     *    - Update our current BSS information from scan data
     */
    //wlan_scan_process_results(card);

    rt_free(scanCfgOut);

    return ret;
}

/**
 *  @brief Update the scan entry TSF timestamps to reflect a new association
 *
 *  @param priv         A pointer to wlan_private structure
 *  @param pNewBssDesc  A pointer to the newly associated AP's scan table entry
 *
 *  @return             void
 */
void wlan_scan_update_tsf_timestamps(WlanCard *cardinfo,BSSDescriptor_t * pNewBssDesc)
{
    WlanCard *card=cardinfo;

    u64 newTsfBase;
    //s64 tsfDelta;

    rt_memcpy(&card->ScanTable->networkTSF[0], pNewBssDesc->TimeStamp, sizeof(card->ScanTable->networkTSF));

    //tsfDelta = newTsfBase - pNewBssDesc->networkTSF;

    //wlan_debug3("TSF: Update TSF timestamps, 0x%x -> 0x%x\n",
    //NewBssDesc->networkTSF, newTsfBase);
    //card->ScanTable->networkTSF += tsfDelta;
}

int wlan_execute_scan_cmd(WlanCard *cardinfo, wlan_scan_cmd_config  *pScanCfg_in)
{
    HostCmd_DS_COMMAND *CmdPtr=NULL;
    HostCmd_DS_802_11_SCAN *pScan =NULL;
    wlan_scan_cmd_config *pScanCfg;
    WlanCard *card=cardinfo;
    int ret;

    CmdPtr=rt_malloc(512);//(sizeof(HostCmd_DS_COMMAND));
    if(CmdPtr==NULL)
    {
        WlanDebug(WlanErr,"alloc memory faild for Scan cmd\r\n");
        return WLAN_STATUS_FAILURE;

    }
    /* Set sequence number, command and INT option */
    card->SeqNum++;
    CmdPtr->SeqNum =card->SeqNum;
    CmdPtr->Command = HostCmd_CMD_802_11_SCAN;
    CmdPtr->Result = 0;

    pScan=&CmdPtr->params.scan;
    pScanCfg = (wlan_scan_cmd_config *) pScanCfg_in;

    /* Set fixed field variables in scan command */
    pScan->BSSType = pScanCfg->bssType;
    rt_memcpy(pScan->BSSID, pScanCfg->specificBSSID, sizeof(pScan->BSSID));
    rt_memcpy(pScan->TlvBuffer, pScanCfg->tlvBuffer, pScanCfg->tlvBufferLen);

    /* Size is equal to the sizeof(fixed portions) + the TLV len + header */
    CmdPtr->Size = (sizeof(pScan->BSSType)
                    + sizeof(pScan->BSSID)
                    + pScanCfg->tlvBufferLen + S_DS_GEN);

    ret=WlanExecuteCommand(cardinfo,CmdPtr);

    if (ret)
    {
        WlanDebug(WlanErr, "Scan cmd failed\r\n");
        ret = WLAN_STATUS_FAILURE;
    }
    rt_free(CmdPtr);
    return WLAN_STATUS_SUCCESS;
}

/**
 *  @brief Store a beacon or probe response for a BSS returned in the scan
 *
 *  Store a new scan response or an update for a previous scan response.  New
 *    entries need to verify that they do not exceed the total amount of
 *    memory allocated for the table.

 *  Replacement entries need to take into consideration the amount of space
 *    currently allocated for the beacon/probe response and adjust the entry
 *    as needed.
 *
 *  A small amount of extra pad (SCAN_BEACON_ENTRY_PAD) is generally reserved
 *    for an entry in case it is a beacon since a probe response for the
 *    network will by larger per the standard.  This helps to reduce the
 *    amount of memory copying to fit a new probe response into an entry
 *    already occupied by a network's previously stored beacon.
 *
 *  @param priv       A pointer to wlan_private structure
 *  @param beaconIdx  Index in the scan table to store this entry; may be
 *                    replacing an older duplicate entry for this BSS
 *  @param numInTable Number of entries currently in the table
 *  @param pNewBeacon Pointer to the new beacon/probe response to save
 *
 *  @return           void
 */

void wlan_ret_802_11_scan_store_beacon(WlanCard *cardinfo, BSSDescriptor_t * pNewBeacon)
{
    WlanCard *card=cardinfo;
    u8 *pBcnStore=NULL;
    int newBcnSize;

    newBcnSize = pNewBeacon->beaconBufSize;

    // pBcnStore=rt_malloc(newBcnSize);
    pBcnStore=beaconbuf;
    if(pBcnStore==NULL)
    {
        WlanDebug(WlanErr, "wlan_ret_802_11_scan_store_beacon failed alloc memory\r\n");
        return;
    }
    rt_memcpy(pBcnStore,
              pNewBeacon->pBeaconBuf, pNewBeacon->beaconBufSize);
    pNewBeacon->pBeaconBuf=pBcnStore;
    return ;
}

int InterruptScanResult (WlanCard *cardinfo, u8 ** pBeaconInfo, int *bytesLeft)
{
    WlanCard *card=cardinfo;
    ScanResultItem * item=NULL;
    u32 itemcount=0;
    IEEEtypes_ElementId_e elemID;
    IEEEtypes_DsParamSet_t *pDS;
    IEEEtypes_CapInfo_t Cap;
    IEEEtypes_CapInfo_t *pCap = &Cap;
    WLAN_802_11_FIXED_IEs fixedIE;
    u8 *pCurrentPtr;
    u8 *pRate;
    u8 elemLen;
    u16 totalIeLen;
    u8 bytesToCopy;
    u8 rateSize;
    u16 beaconSize;
    BOOLEAN foundDataRateIE;
    int bytesLeftForCurrentBeacon;
    IEEEtypes_VendorSpecific_t *pVendorIe;
    const u8 wpa_oui[4] = { 0x00, 0x50, 0xf2, 0x01 };
    IEEEtypes_CountryInfoSet_t *pcountryinfo;
    u8  wpaflag=0;
    u8  wpa2flag=0;
    foundDataRateIE = FALSE;
    rateSize = 0;
    beaconSize = 0;

    if (*bytesLeft >= sizeof(beaconSize))
    {
        /* Extract & convert beacon size from the command buffer */
        rt_memcpy(&beaconSize, *pBeaconInfo, sizeof(beaconSize));
        *bytesLeft -= sizeof(beaconSize);
        *pBeaconInfo += sizeof(beaconSize);
    }

    if ((beaconSize == 0) || (beaconSize > *bytesLeft)||(cardinfo->ScanResultInfo==NULL))
    {

        *pBeaconInfo += *bytesLeft;
        *bytesLeft = 0;

        if(item==NULL)
        {
            WlanDebug(WlanErr, "SCAN_RESP:there is not buffer to store scan information\n");
        }
        return WLAN_STATUS_FAILURE;
    }

    itemcount=card->ScanResultcount;
    item=card->ScanResultInfo+itemcount;
    if(((itemcount+1)*sizeof(ScanResultItem)) >card->ScanResulMuxsize)
    {
        *pBeaconInfo += *bytesLeft;
        *bytesLeft = 0;
        return WLAN_STATUS_FAILURE;
    }
    /* Initialize the current working beacon pointer for this BSS iteration */
    pCurrentPtr = *pBeaconInfo;

    /* Advance the return beacon pointer past the current beacon */
    *pBeaconInfo += beaconSize;
    *bytesLeft -= beaconSize;

    bytesLeftForCurrentBeacon = beaconSize;

    rt_memcpy(item->MacAddress, pCurrentPtr, MRVDRV_ETH_ADDR_LEN);

    pCurrentPtr += MRVDRV_ETH_ADDR_LEN;
    bytesLeftForCurrentBeacon -= MRVDRV_ETH_ADDR_LEN;

    if (bytesLeftForCurrentBeacon < 12)
    {
        WlanDebug(WlanErr,"ParzeBSSDescription: Not enough bytes left\n");
        return WLAN_STATUS_FAILURE;
    }
    /* RSSI is 1 byte long */
    item->Rssi = ((LONG) (*pCurrentPtr));
    pCurrentPtr += 1;
    bytesLeftForCurrentBeacon -= 1;

    /* time stamp is 8 bytes long: skip this field*/
    //hexdump("timestamp:",(char*)pCurrentPtr,8);
    pCurrentPtr += 8;
    bytesLeftForCurrentBeacon -= 8;

    /* beacon interval is 2 bytes long skip this field*/
    //hexdump("interval:",pCurrentPtr,2);
    pCurrentPtr += 2;
    bytesLeftForCurrentBeacon -= 2;

    /* capability information is 2 bytes long */
    rt_memcpy(pCap, pCurrentPtr, 2);
    pCurrentPtr += 2;
    bytesLeftForCurrentBeacon -= 2;

    if (pCap->Privacy)
    {
        item->Privacy = Wlan802_11PrivFilter8021xWEP;
    }
    else
    {
        item->Privacy = Wlan802_11PrivFilterAcceptAll;
    }

    if (pCap->Ibss == 1)
    {
        item->InfrastructureMode = Wlan802_11IBSS;
    }
    else
    {
        item->InfrastructureMode = Wlan802_11Infrastructure;
    }

    /* process variable IE */
    while (bytesLeftForCurrentBeacon >= 2)
    {
        elemID = (IEEEtypes_ElementId_e) (*((u8 *) pCurrentPtr));
        elemLen = *((u8 *) pCurrentPtr + 1);
        totalIeLen = elemLen + sizeof(IEEEtypes_Header_t);

        if (bytesLeftForCurrentBeacon < elemLen)
        {
            WlanDebug(WlanMsg,"ParzeBSSD: Error in processing IE bytes left < IE length\n");
            bytesLeftForCurrentBeacon = 0;
            continue;
        }

        switch (elemID)
        {
        case SSID:
            item->Ssid.SsidLength = elemLen<WLAN_MAX_SSID_LENGTH?elemLen:WLAN_MAX_SSID_LENGTH;
            rt_memcpy(item->Ssid.Ssid, (pCurrentPtr + 2), item->Ssid.SsidLength);
            item->Ssid.Ssid[item->Ssid.SsidLength]=0;
            WlanDebug(WlanMsg,"SSID %s\n",item->Ssid.Ssid);
            break;

        case SUPPORTED_RATES:
            rt_memcpy(item->DataRates, pCurrentPtr + 2, elemLen);
            rt_memcpy(item->SupportedRates, pCurrentPtr + 2, elemLen);

            rateSize = elemLen;
            foundDataRateIE = TRUE;
            break;

        case EXTRA_IE:
            //WlanDebug(WlanMsg,"ParzeBSSD: EXTRA_IE Found!\n");
            break;

        case FH_PARAM_SET:
            break;

        case DS_PARAM_SET:
            pDS = (IEEEtypes_DsParamSet_t *) pCurrentPtr;
            item->Channel = pDS->CurrentChan;
            break;

        case CF_PARAM_SET:
            break;

        case IBSS_PARAM_SET:
            break;

            /* Handle Country Info IE */
        case COUNTRY_INFO:
            break;
        case ERP_INFO:
            break;
        case EXTENDED_SUPPORTED_RATES:
            break;

        case VENDOR_SPECIFIC_221:
            pVendorIe = (IEEEtypes_VendorSpecific_t *) pCurrentPtr;
            if (memcmp(pVendorIe->VendHdr.Oui, wpa_oui, sizeof(wpa_oui))==0)
                wpaflag=1;

            hexdump("VendorIE:",pCurrentPtr,totalIeLen);
            break;
        case RSN_IE:

            wpa2flag=1;
            hexdump("RsnIE:",pCurrentPtr,totalIeLen);
            break;
        }

        if(wpa2flag==1)
        {
            item->Security=WPA2_PSK;
        }
        else if(wpaflag==1)
        {
            item->Security=WPA_PSK;
        }
        else
        {
            item->Security=WEP;
        }
        pCurrentPtr += (elemLen + 2);

        /* need to account for IE ID and IE Len */
        bytesLeftForCurrentBeacon -= (elemLen + 2);

    }                           /* while (bytesLeftForCurrentBeacon > 2) */
    card->ScanResultcount++;
    return WLAN_STATUS_SUCCESS;
}

int WlanparserScanResult(WlanCard *cardinfo, HostCmd_DS_COMMAND * resp)
{
    WlanCard *card=cardinfo;
    HostCmd_DS_802_11_SCAN_RSP *pScan;
    BSSDescriptor_t newBssEntry;
    MrvlIEtypes_Data_t *pTlv;
    MrvlIEtypes_TsfTimestamp_t *pTsfTlv;
    u8 *pBssInfo;
    u16 scanRespSize;
    int bytesLeft;
    int numInTable;
    int bssIdx;
    int idx;

    BOOLEAN bgScanResp;

    bgScanResp = (resp->Command == HostCmd_RET_802_11_BG_SCAN_QUERY);
    if (bgScanResp)
    {
        pScan = &resp->params.bgscanqueryresp.scanresp;
    }
    else
    {
        pScan = &resp->params.scanresp;
    }

    if (pScan->NumberOfSets > MRVDRV_MAX_BSSID_LIST)
    {
        WlanDebug( WlanErr,"parserScan: Invalid number of AP returned (%d)!!\n",
                   pScan->NumberOfSets);
        return WLAN_STATUS_FAILURE;
    }

    bytesLeft = pScan->BSSDescriptSize;
    WlanDebug( WlanCmd,"parserScan: BSSDescriptSize %d\n", bytesLeft);

    scanRespSize = resp->Size;

    WlanDebug( WlanCmd,"parserScan: returned %d APs before parsing\n",
               pScan->NumberOfSets);

    pBssInfo = pScan->BssDescAndTlvBuffer;

    /*
     *  Process each scan response returned (pScan->NumberOfSets).  Save
     *    the information in the newBssEntry and then insert into the
     *    driver scan table either as an update to an existing entry
     *    or as an addition at the end of the table
     */
    for (idx = 0; idx < pScan->NumberOfSets && bytesLeft; idx++)
    {
        /* Zero out the newBssEntry we are about to store info in */
        // memset(&newBssEntry, 0x00, sizeof(newBssEntry));

        /* Process the data fields and IEs returned for this BSS */
        if ((InterruptScanResult(card,
                                 &pBssInfo,
                                 &bytesLeft) == WLAN_STATUS_SUCCESS))
        {

        }
        else
        {

            /* Error parsing/interpreting the scan response, skipped */
            WlanDebug( WlanMsg,"SCAN_RESP: "
                       "InterpretBSSDescriptionWithIE returned ERROR\n");
        }
    }

    WlanDebug( WlanMsg, "SCAN_RESP: Scanned %2d\n",pScan->NumberOfSets);

    return WLAN_STATUS_SUCCESS;
}

int SendSpecificBSSIDScan(WlanCard *cardinfo, u8 *bssid, char *ssid, int channel)
{
    WlanCard *card=cardinfo;
    wlan_scan_cfg scanCfg;
    int ssidlen;

    if (bssid == NULL || ssid==NULL)
        return WLAN_STATUS_FAILURE;

    rt_memset(&scanCfg, 0x00, sizeof(scanCfg));
    if(bssid != NULL)
    {
        rt_memcpy(scanCfg.specificBSSID, bssid, sizeof(scanCfg.specificBSSID));
        WlanDebug( WlanCmd, "filter scan based on BSSID\n");
    }
    else if(ssid != NULL)
    {
        ssidlen=rt_strlen(ssid);
        scanCfg.ssidList.maxLen=ssidlen;
        rt_memcpy(scanCfg.ssidList.ssid, ssid, ssidlen);
        WlanDebug( WlanCmd,"filter scan based on SSID %d\n",ssidlen);
    }

    if(channel >0 && channel <= 14)
    {
        scanCfg.chanList.validflag=1;
        scanCfg.chanList.chanNumber=channel;
    }
    else
    {
        scanCfg.chanList.validflag = 0;
    }

    scanCfg.bssType = WLAN_SCAN_BSS_TYPE_BSS;
    scanCfg.chanList.radioType=HostCmd_SCAN_RADIO_TYPE_BG;		//!< Radio type: 'B/G' Band = 0, 'A' Band = 1
    scanCfg.chanList.scanType = HostCmd_SCAN_TYPE_ACTIVE;			//!< Scan type: Active = 0, Passive =

    if(card->ScanPurpose == ScanIdle)
    {
        card->ScanPurpose=ScanFilter;
        wlan_scan_networks(card, &scanCfg);
        card->ScanPurpose=ScanMultichs;
    }
    else
    {
        return WLAN_STATUS_FAILURE;
    }

    return WLAN_STATUS_SUCCESS;
}

int SendScanToGetAPInfo(WlanCard *cardinfo, char *buf,int buflen)
{
    WlanCard *card=cardinfo;
    wlan_scan_cfg scanCfg;

    if(buf==NULL||buflen<=0)
    {
        WlanDebug( WlanErr,"all channel scan did not start: there is not Rx buffer\n");
        return WLAN_STATUS_FAILURE;
    }

    card->ScanResulMuxsize=buflen;
    card->ScanResultInfo=(ScanResultItem *)buf;
    card->ScanResultcount=0;

    rt_memset(&scanCfg, 0x00, sizeof(scanCfg));
    scanCfg.bssType=WLAN_SCAN_BSS_TYPE_BSS;
    scanCfg.chanList.validflag=0;
    scanCfg.chanList.radioType=HostCmd_SCAN_RADIO_TYPE_BG;               //!< Radio type: 'B/G' Band = 0, 'A' Band = 1
    scanCfg.chanList.scanType=HostCmd_SCAN_TYPE_ACTIVE;                //!< Scan type: Active = 0, Passive =
    if(card->ScanPurpose==ScanIdle)
    {
        card->ScanPurpose=ScanMultichs;
        wlan_scan_networks(card, &scanCfg);
        card->ScanPurpose=ScanIdle;
    }
    else
    {
        return WLAN_STATUS_FAILURE;
    }
    return WLAN_STATUS_SUCCESS;
}

int SendSpecificScanConfig(WlanCard *cardinfo, WlanConfig *config, char* buf, int buflen)
{
    WlanCard *card=cardinfo;
    wlan_scan_cfg scanCfg;
    int ssidlen;

    card->ScanResulMuxsize=buflen;
    card->ScanResultInfo=(ScanResultItem *)buf;
    card->ScanResultcount=0;

    rt_memset(&scanCfg, 0x00, sizeof(scanCfg));
    if (config->SSID[0] != '\0')
    {
        ssidlen=rt_strlen(config->SSID);
        scanCfg.ssidList.maxLen=ssidlen;
        rt_memcpy(scanCfg.ssidList.ssid, config->SSID, ssidlen);
    }

    if(config->channel >0 && config->channel <= 14)
    {
        scanCfg.chanList.validflag = 1;
        scanCfg.chanList.chanNumber = config->channel;
    }
    else
    {
        scanCfg.chanList.validflag = 0;
    }

    scanCfg.bssType = WLAN_SCAN_BSS_TYPE_BSS;
    scanCfg.chanList.radioType=HostCmd_SCAN_RADIO_TYPE_BG;		//!< Radio type: 'B/G' Band = 0, 'A' Band = 1
    scanCfg.chanList.scanType = HostCmd_SCAN_TYPE_ACTIVE;			//!< Scan type: Active = 0, Passive =

    if(card->ScanPurpose == ScanIdle)
    {
        card->ScanPurpose=ScanFilter;
        wlan_scan_networks(card, &scanCfg);
        card->ScanPurpose = ScanIdle;
    }
    else
    {
        return WLAN_STATUS_FAILURE;
    }

    return WLAN_STATUS_SUCCESS;
}

