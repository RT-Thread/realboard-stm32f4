/** @file wlan_11d.c
  * @brief This file contains functions for 802.11D.
  * 
  *  Copyright © Marvell International Ltd. and/or its affiliates, 2003-2006
  */
/********************************************************
Change log:
	10/04/05: Add Doxygen format comments
	
********************************************************/
#include	"include.h"

/********************************************************
		Local Variables
********************************************************/
#define TX_PWR_DEFAULT	10

static region_code_mapping_t region_code_mapping[] = {
    {"US ", 0x10},              /* US FCC      */
    {"CA ", 0x20},              /* IC Canada   */
    {"SG ", 0x10},              /* Singapore   */
    {"EU ", 0x30},              /* ETSI        */
    {"AU ", 0x30},              /* Australia   */
    {"KR ", 0x30},              /* Republic Of Korea */
    {"ES ", 0x31},              /* Spain       */
    {"FR ", 0x32},              /* France      */
    {"JP ", 0x40},              /* Japan       */
    {"JP ", 0x41},              /* Japan       */
};

/********************************************************
		Global Variables
********************************************************/
/* Following 2 structure defines the supported channels */
const CHANNEL_FREQ_POWER channel_freq_power_UN_BG[] = {
    {1, 2412, TX_PWR_DEFAULT},
    {2, 2417, TX_PWR_DEFAULT},
    {3, 2422, TX_PWR_DEFAULT},
    {4, 2427, TX_PWR_DEFAULT},
    {5, 2432, TX_PWR_DEFAULT},
    {6, 2437, TX_PWR_DEFAULT},
    {7, 2442, TX_PWR_DEFAULT},
    {8, 2447, TX_PWR_DEFAULT},
    {9, 2452, TX_PWR_DEFAULT},
    {10, 2457, TX_PWR_DEFAULT},
    {11, 2462, TX_PWR_DEFAULT},
    {12, 2467, TX_PWR_DEFAULT},
    {13, 2472, TX_PWR_DEFAULT},
    {14, 2484, TX_PWR_DEFAULT}
};

const CHANNEL_FREQ_POWER channel_freq_power_UN_AJ[] = {
    {8, 5040, TX_PWR_DEFAULT},
    {12, 5060, TX_PWR_DEFAULT},
    {16, 5080, TX_PWR_DEFAULT},
    {34, 5170, TX_PWR_DEFAULT},
    {38, 5190, TX_PWR_DEFAULT},
    {42, 5210, TX_PWR_DEFAULT},
    {46, 5230, TX_PWR_DEFAULT},
    {36, 5180, TX_PWR_DEFAULT},
    {40, 5200, TX_PWR_DEFAULT},
    {44, 5220, TX_PWR_DEFAULT},
    {48, 5240, TX_PWR_DEFAULT},
    {52, 5260, TX_PWR_DEFAULT},
    {56, 5280, TX_PWR_DEFAULT},
    {60, 5300, TX_PWR_DEFAULT},
    {64, 5320, TX_PWR_DEFAULT},
    {100, 5500, TX_PWR_DEFAULT},
    {104, 5520, TX_PWR_DEFAULT},
    {108, 5540, TX_PWR_DEFAULT},
    {112, 5560, TX_PWR_DEFAULT},
    {116, 5580, TX_PWR_DEFAULT},
    {120, 5600, TX_PWR_DEFAULT},
    {124, 5620, TX_PWR_DEFAULT},
    {128, 5640, TX_PWR_DEFAULT},
    {132, 5660, TX_PWR_DEFAULT},
    {136, 5680, TX_PWR_DEFAULT},
    {140, 5700, TX_PWR_DEFAULT},
    {149, 5745, TX_PWR_DEFAULT},
    {153, 5765, TX_PWR_DEFAULT},
    {157, 5785, TX_PWR_DEFAULT},
    {161, 5805, TX_PWR_DEFAULT},
    {165, 5825, TX_PWR_DEFAULT},
/*	{240, 4920, TX_PWR_DEFAULT}, 
	{244, 4940, TX_PWR_DEFAULT}, 
	{248, 4960, TX_PWR_DEFAULT}, 
	{252, 4980, TX_PWR_DEFAULT}, 
channels for 11J JP 10M channel gap */
};

extern CHANNEL_FREQ_POWER *wlan_get_region_cfp_table(u8 region,
                                                     u8 band, int *cfp_no);

/********************************************************
		Local Functions
********************************************************/
/** 
 *  @brief This function convert Region string to code integer
 *  @param region     region string
 *  @return 	      region id
*/
static u8
wlan_region_2_code(s8 * region)
{
    u8 i;
    u8 size = sizeof(region_code_mapping) / sizeof(region_code_mapping_t);

    for (i = 0; region[i] && i < COUNTRY_CODE_LEN; i++)
        region[i] = toupper(region[i]);

    for (i = 0; i < size; i++) {
        if (!memcmp(region, region_code_mapping[i].region, COUNTRY_CODE_LEN))
            return (region_code_mapping[i].code);
    }

    /* default is US */
    return (region_code_mapping[0].code);
}

/** 
 *  @brief This function converts interger code to region string
 *  @param code       region code
 *  @return 	      region string
*/
static u8 *
wlan_code_2_region(u8 code)
{
    u8 i;
    u8 size = sizeof(region_code_mapping) / sizeof(region_code_mapping_t);
    for (i = 0; i < size; i++) {
        if (region_code_mapping[i].code == code)
            return (region_code_mapping[i].region);
    }
    /* default is US */
    return (region_code_mapping[0].region);
}

/** 
 *  @brief This function finds the NoOfChan-th chan after the firstChan
 *  @param band       band
 *  @param firstChan  first channel number
 *  @param NoOfChan   number of channels
 *  @return 	      the NoOfChan-th chan number
*/
static BOOLEAN
wlan_get_chan_11d(u8 band, u8 firstChan, u8 NoOfChan, u8 * chan)
/*find the NoOfChan-th chan after the firstChan*/
{
    u8 i;
    const CHANNEL_FREQ_POWER *cfp;
    u8 cfp_no;

    {
        cfp = channel_freq_power_UN_BG;
        cfp_no = sizeof(channel_freq_power_UN_BG) /
            sizeof(CHANNEL_FREQ_POWER);
    }

    for (i = 0; i < cfp_no; i++) {
        if ((cfp + i)->Channel == firstChan) {
            break;
        }
    }

    if (i < cfp_no) {
        /*if beyond the boundary */
        if (i + NoOfChan < cfp_no) {
            *chan = (cfp + i + NoOfChan)->Channel;
            return TRUE;
        }
    }

    return FALSE;
}

/** 
 *  @brief This function Checks if chan txpwr is learned from AP/IBSS
 *  @param chan                 chan number
 *  @param parsed_region_chan   pointer to parsed_region_chan_11d_t     
 *  @return 	                TRUE; FALSE
*/
BOOLEAN
wlan_channel_known_11d(u8 chan, parsed_region_chan_11d_t * parsed_region_chan)
{
    chan_power_11d_t *chanPwr = parsed_region_chan->chanPwr;
    u8 NoOfChan = parsed_region_chan->NoOfChan;
    u8 i = 0;

 
    for (i = 0; i < NoOfChan; i++) {
        if (chan == chanPwr[i].chan) {
            return TRUE;
        }
    }

    return FALSE;
}

/********************************************************
		Global Functions
********************************************************/

/** 
 *  @brief This function Converts chan to frequency
 *  @param chan                 channel number
 *  @param band                 band
 *  @return 	                channel frequency
*/
u32
chan_2_freq(u8 chan, u8 band)
{
    const CHANNEL_FREQ_POWER *cf;
    u16 cnt;
    u16 i;
    u32 freq = 0;

    {
        cf = channel_freq_power_UN_BG;
        cnt = sizeof(channel_freq_power_UN_BG) / sizeof(CHANNEL_FREQ_POWER);
    }

    for (i = 0; i < cnt; i++) {
        if (chan == cf[i].Channel)
            freq = cf[i].Freq;
    }


    return freq;
}
/** 
 *  @brief This function gets if 11D is enabled
 *  @param priv       pointer to wlan_private
 *  @return 	      ENABLE_11D;DISABLE_11D
*/
state_11d_t wlan_get_state_11d(WlanCard *cardinfo)
{
    WlanCard *card = cardinfo;
    wlan_802_11d_state_t *state = &cardinfo->State11D;
    return (state->Enable11D);
}
/** 
 *  @brief This function generates domaininfo from parsed_region_chan
 *  @param parsed_region_chan   pointer to parsed_region_chan_11d_t
 *  @param domaininfo           pointer to wlan_802_11d_domain_reg_t
 *  @return 	                WLAN_STATUS_SUCCESS
*/
int
wlan_generate_domain_info_11d(parsed_region_chan_11d_t * parsed_region_chan,
                              wlan_802_11d_domain_reg_t * domaininfo)
{
    u8 NoOfSubband = 0;

    u8 NoOfChan = parsed_region_chan->NoOfChan;
    u8 NoOfParsedChan = 0;

    u8 firstChan = 0, nextChan = 0, maxPwr = 0;

    u8 i, flag = 0;



    memcpy(domaininfo->CountryCode, parsed_region_chan->CountryCode,COUNTRY_CODE_LEN);

    for (i = 0; i < NoOfChan; i++) {
        if (!flag) {
            flag = 1;
            nextChan = firstChan = parsed_region_chan->chanPwr[i].chan;
            maxPwr = parsed_region_chan->chanPwr[i].pwr;
            NoOfParsedChan = 1;
            continue;
        }

        if (parsed_region_chan->chanPwr[i].chan == nextChan + 1 &&
            parsed_region_chan->chanPwr[i].pwr == maxPwr) {
            nextChan++;
            NoOfParsedChan++;
        } else {
            domaininfo->Subband[NoOfSubband].FirstChan = firstChan;
            domaininfo->Subband[NoOfSubband].NoOfChan = NoOfParsedChan;
            domaininfo->Subband[NoOfSubband].MaxTxPwr = maxPwr;
            NoOfSubband++;
            NoOfParsedChan = 1;
            nextChan = firstChan = parsed_region_chan->chanPwr[i].chan;
            maxPwr = parsed_region_chan->chanPwr[i].pwr;
        }
    }

    if (flag) {
        domaininfo->Subband[NoOfSubband].FirstChan = firstChan;
        domaininfo->Subband[NoOfSubband].NoOfChan = NoOfParsedChan;
        domaininfo->Subband[NoOfSubband].MaxTxPwr = maxPwr;
        NoOfSubband++;
    }
    domaininfo->NoOfSubband = NoOfSubband;


   hexdump("11D:domaininfo:", (char *) domaininfo,
           COUNTRY_CODE_LEN + 1 +
            sizeof(IEEEtypes_SubbandSet_t) * NoOfSubband);
    return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief This function generates parsed_region_chan from Domain Info learned from AP/IBSS
 *  @param region_chan          pointer to REGION_CHANNEL
 *  @param *parsed_region_chan  pointer to parsed_region_chan_11d_t
 *  @return 	                N/A
*/
void wlan_generate_parsed_region_chan_11d(REGION_CHANNEL * region_chan,
                                     parsed_region_chan_11d_t *
                                     parsed_region_chan)
{
    u8 i;
    const CHANNEL_FREQ_POWER *cfp;

    if (region_chan == NULL) {
        WlanDebug(WlanErr,"11D: region_chan is NULL\n");
        return;
    }

    cfp = region_chan->CFP;
    if (cfp == NULL) {
        WlanDebug(WlanErr,"11D: cfp equal NULL \n");
        return;
    }

    parsed_region_chan->band = region_chan->Band;
    parsed_region_chan->region = region_chan->Region;
    memcpy(parsed_region_chan->CountryCode,
           wlan_code_2_region(region_chan->Region), COUNTRY_CODE_LEN);

    WlanDebug(WlanMsg,"11D: region[0x%x] band[%d]\n", parsed_region_chan->region,
           parsed_region_chan->band);

    for (i = 0; i < region_chan->NrCFP; i++, cfp++) {
        parsed_region_chan->chanPwr[i].chan = cfp->Channel;
        parsed_region_chan->chanPwr[i].pwr = cfp->MaxTxPower;
        WlanDebug(WlanMsg,"11D: Chan[%d] Pwr[%d]\n",
               parsed_region_chan->chanPwr[i].chan,
               parsed_region_chan->chanPwr[i].pwr);
    }
    parsed_region_chan->NoOfChan = region_chan->NrCFP;

    WlanDebug(WlanMsg,"11D: NoOfChan[%d]\n", parsed_region_chan->NoOfChan);
    return;
}

/** 
 *  @brief generate parsed_region_chan from Domain Info learned from AP/IBSS
 *  @param region               region ID
 *  @param band                 band
 *  @param chan                 chan
 *  @return 	                TRUE;FALSE
*/
BOOLEAN
wlan_region_chan_supported_11d(u8 region, u8 band, u8 chan)
{
    CHANNEL_FREQ_POWER *cfp;
    int cfp_no;
    u8 idx;

    if ((cfp = wlan_get_region_cfp_table(region, band, &cfp_no)) == NULL) {
        return FALSE;
    }

    for (idx = 0; idx < cfp_no; idx++) {
        if (chan == (cfp + idx)->Channel) {
            /* If Mrvl Chip Supported? */
            if ((cfp + idx)->Unsupported) {
                return FALSE;
            } else {
                return TRUE;
            }
        }
    }

    /*chan is not in the region table */
    return FALSE;
}

/** 
 *  @brief This function checks if chan txpwr is learned from AP/IBSS
 *  @param chan                 chan number
 *  @param parsed_region_chan   pointer to parsed_region_chan_11d_t     
 *  @return 	                WLAN_STATUS_SUCCESS       
*/
int
wlan_parse_domain_info_11d(IEEEtypes_CountryInfoFullSet_t * CountryInfo,
                           u8 band,
                           parsed_region_chan_11d_t * parsed_region_chan)
{
    u8 NoOfSubband, NoOfChan;
    u8 lastChan, firstChan, curChan;
    u8 region;

    u8 idx = 0;                 /*chan index in parsed_region_chan */

    u8 j, i;



    /*Validation Rules:
       1. Valid Region Code
       2. First Chan increment
       3. Channel range no overlap
       4. Channel is valid?
       5. Channel is supported by Region?
       6. Others
     */

   hexdump("CountryInfo:", (s8 *) CountryInfo, 30);

    if ((*(CountryInfo->CountryCode)) == 0 ||
        (CountryInfo->Len <= COUNTRY_CODE_LEN)) {
        /* No region Info or Wrong region info: treat as No 11D info */

        return WLAN_STATUS_SUCCESS;
    }

    /*Step1: check region_code */
    parsed_region_chan->region = region =
        wlan_region_2_code(CountryInfo->CountryCode);

    WlanDebug(WlanMsg,"regioncode=%x\n", (u8) parsed_region_chan->region);

    parsed_region_chan->band = band;

    rt_memcpy(parsed_region_chan->CountryCode, CountryInfo->CountryCode,
           COUNTRY_CODE_LEN);

    NoOfSubband = (CountryInfo->Len - COUNTRY_CODE_LEN) /
        sizeof(IEEEtypes_SubbandSet_t);

    for (j = 0, lastChan = 0; j < NoOfSubband; j++) {

        if (CountryInfo->Subband[j].FirstChan <= lastChan) {
            /*Step2&3. Check First Chan Num increment and no overlap */
             WlanDebug(WlanMsg,"11D: Chan[%d>%d] Overlap\n",
                   CountryInfo->Subband[j].FirstChan, lastChan);
            continue;
        }

        firstChan = CountryInfo->Subband[j].FirstChan;
        NoOfChan = CountryInfo->Subband[j].NoOfChan;

        for (i = 0; idx < MAX_NO_OF_CHAN && i < NoOfChan; i++) {
            /*step4: channel is supported? */

            if (wlan_get_chan_11d(band, firstChan, i, &curChan)
                == FALSE) {
                /* Chan is not found in UN table */
                 WlanDebug(WlanErr,"chan is not supported: %d \n", i);
                break;
            }

            lastChan = curChan;

            /*step5: We don't need to Check if curChan is supported by mrvl in region */
            parsed_region_chan->chanPwr[idx].chan = curChan;
            parsed_region_chan->chanPwr[idx].pwr =
                CountryInfo->Subband[j].MaxTxPwr;
            idx++;
        }

        /*Step6: Add other checking if any */

    }

    parsed_region_chan->NoOfChan = idx;

    WlanDebug(WlanMsg,"NoOfChan=%x\n", parsed_region_chan->NoOfChan);
    hexdump("11D:parsed_region_chan:", (s8 *) parsed_region_chan,
           2 + COUNTRY_CODE_LEN + sizeof(parsed_region_chan_11d_t) * idx);
    return WLAN_STATUS_SUCCESS;
}

/** 
 *  @brief This function calculates the scan type for channels
 *  @param chan                 chan number
 *  @param parsed_region_chan   pointer to parsed_region_chan_11d_t     
 *  @return 	                PASSIVE if chan is unknown; ACTIVE if chan is known
*/
u8
wlan_get_scan_type_11d(u8 chan, parsed_region_chan_11d_t * parsed_region_chan)
{
    u8 scan_type = HostCmd_SCAN_TYPE_PASSIVE;

 

    if (wlan_channel_known_11d(chan, parsed_region_chan)) {
         WlanDebug(WlanMsg, "11D: Found and do Active Scan\n");
        scan_type = HostCmd_SCAN_TYPE_ACTIVE;
    } else {
         WlanDebug(WlanMsg,"11D: Not Find and do Passive Scan\n");
    }

    return scan_type;

}

/** 
 *  @brief This function setups scan channels
 *  @param priv       pointer to wlan_private
 *  @param band       band
 *  @return 	      WLAN_STATUS_SUCCESS
*/
int wlan_set_universaltable(WlanCard *cardinfo, u8 band)
{
     WlanCard *card= cardinfo;
    u16 size = sizeof(CHANNEL_FREQ_POWER);

    rt_memset(&card->universal_channel, 0, sizeof(card->universal_channel));

    {
        card->universal_channel.NrCFP =sizeof(channel_freq_power_UN_BG) / size;
        WlanDebug(WlanMsg,"11D: BG-band NrCFP=%d\n",card->universal_channel.NrCFP);

        card->universal_channel.CFP = channel_freq_power_UN_BG;
        card->universal_channel.Valid = TRUE;
        card->universal_channel.Region = UNIVERSAL_REGION_CODE;
        card->universal_channel.Band = band;
       
    }
    return WLAN_STATUS_SUCCESS;
}
