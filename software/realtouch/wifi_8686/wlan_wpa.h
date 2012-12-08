#ifndef __WLAN_WPA_H__
#define __WLAN_WPA_H__

#include <rtthread.h>

struct wlan_wpa_context
{
	int (*supplicant_init)(rt_uint32_t key_mgmt, rt_uint32_t cipher, const rt_uint8_t *ie_info, rt_uint32_t ie_info_len);
	int (*calculate_pmk)(rt_uint8_t* password,rt_uint8_t* ssid);
	int (*eapol_input)(rt_uint8_t* ptr, rt_uint32_t len, rt_uint8_t *hwaddr);
	int (*done)(void);
};

int wlan_wpa_supplicant_init(rt_uint32_t key_mgmt, rt_uint32_t cipher, const rt_uint8_t* ie, rt_uint32_t ie_len);
int wlan_wpa_calculate_pmk(rt_uint8_t *password, rt_uint8_t *ssid);
int wlan_wpa_eapol_input(rt_uint8_t* ptr, rt_uint32_t len, rt_uint8_t *hwaddr);

int wlan_wpa_init(void);
int wlan_wpa_done(void);

void wlan_wpa_context_register(const struct wlan_wpa_context* ctx);

#endif

