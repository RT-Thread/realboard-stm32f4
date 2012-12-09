/* WIFI API on ART board */
#ifndef __LIB_WIFI_H__
#define __LIB_WIFI_H__

#include <rtthread.h>
#include <inttypes.h>

#ifdef RT_USING_WIFI
#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Initialize Wifi sub-system
 */
int wlan_system_init(void);

/* Start Wifi connection with passphrase
 * the most secure supported mode will be automatically selected
 *
 * param ssid: Pointer to the SSID string.
 * param passphrase: Passphrase. Valid characters in a passphrase
 *        must be between ASCII 32-126 (decimal).
 */
int wlan_begin(char* ssid, const char *passphrase);

/*
 * Start Wifi connection with saved parameter.
 *
 */
int wlan_begin_auto(void);

/*
 * Disconnect from the network
 *
 * return: one value of wl_status_t enum
 */
int wlan_disconnect(void);

/*
 * Return the current SSID associated with the network
 *
 * return: ssid string
 */
char* wlan_SSID(void);

/*
 * Return the current RSSI /Received Signal Strength in dBm)
 * associated with the network
 *
 * return: signed value
 */
int32_t wlan_RSSI(void);

const char* wlan_BSSID(void);
uint8_t wlan_encryptionType(void);
uint8_t wlan_status(void);

#ifdef __cplusplus
}
#endif
#endif

#endif
