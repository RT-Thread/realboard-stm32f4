/** @file wlan_fw.c
  * @brief This file contains the initialization for FW
  * and HW
  *
  *  Copyright �Marvell International Ltd. and/or its affiliates, 2003-2006
  */
/********************************************************
Change log:
	09/28/05: Add Doxygen format comments
	01/05/06: Add kernel 2.6.x support
	01/11/06: Conditionalize new scan/join functions.
	          Cleanup association response handler initialization.
	01/06/05: Add FW file read
	05/08/06: Remove the 2nd GET_HW_SPEC command and TempAddr/PermanentAddr
	06/30/06: replaced MODULE_PARM(name, type) with module_param(name, type, perm)

********************************************************/

#include	"include.h"
#include	"wlan_debug.h"
#include	"wlan_8686_config.h"

/********************************************************
		Local Variables
********************************************************/



/********************************************************
		Global Variables
********************************************************/

/********************************************************
		Local Functions
********************************************************/
extern int sbi_prog_firmware_image(const u8 * firmware,int firmwarelen);
extern int sbi_download_wlan_fw_image(const u8 * firmware,int firmwarelen);
extern int sbi_verify_fw_download(void);

/**
 *  @brief This function downloads firmware image, gets
 *  HW spec from firmware and set basic parameters to
 *  firmware.
 *
 *  @param priv    A pointer to wlan_private structure
 *  @return 	   WLAN_STATUS_SUCCESS or WLAN_STATUS_FAILURE
 */
int wlan_download_firmware(void)
{
	int ret = WLAN_STATUS_SUCCESS;

	sbi_disable_host_int();
    /* Download the helper */

	ret= sbi_prog_firmware_file(FW_PATH"/helper.bin");
    if (ret)
	{
        WlanDebug(WlanErr,"download helper failed!\n");
        ret = WLAN_STATUS_FAILURE;
        goto done;
    }

    /* Download firmware */
    ret = sbi_download_wlan_fw_file(FW_PATH"/fw.bin");
    if (ret)
	{
       WlanDebug(WlanErr,"download firmware failed\n");
       ret = WLAN_STATUS_FAILURE;
       goto done;
    }

   	/* check if the fimware is downloaded successfully or not */
   	if (sbi_verify_fw_download())
   	{
   		WlanDebug(WlanErr, "FW failed to be active in time!\n");
   		ret = WLAN_STATUS_FAILURE;
   		goto done;
   	}

    sbi_enable_host_int();
    ret = WLAN_STATUS_SUCCESS;

 done:
    return (ret);
}

