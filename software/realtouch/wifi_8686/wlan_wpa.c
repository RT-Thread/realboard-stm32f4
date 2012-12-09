#include <rtthread.h>
#include "wlan_wpa.h"
#include <dfs_posix.h>
#include <rtm.h>

#define FW_PATH         "/sd/firmware"
#define WPA_BIN_FILE	FW_PATH"/wpa.mo"

static const struct wlan_wpa_context* _ctx = RT_NULL;
void wlan_wpa_context_register(const struct wlan_wpa_context* ctx)
{
    _ctx = ctx;
}
RTM_EXPORT(wlan_wpa_context_register);

int wlan_wpa_supplicant_init(rt_uint32_t key_mgmt, rt_uint32_t cipher, const rt_uint8_t* ie, rt_uint32_t ie_len)
{
    if (_ctx != RT_NULL)
    {
        return _ctx->supplicant_init(key_mgmt, cipher, ie, ie_len);
    }

    return 0;
}

int wlan_wpa_calculate_pmk(rt_uint8_t *password, rt_uint8_t *ssid)
{
    if (_ctx != RT_NULL)
    {
        return _ctx->calculate_pmk(password, ssid);
    }

    return 0;
}

int wlan_wpa_eapol_input(rt_uint8_t* ptr, rt_uint32_t len, rt_uint8_t *hwaddr)
{
    if (_ctx != RT_NULL)
    {
        return _ctx->eapol_input(ptr, len, hwaddr);
    }
    return 0;
}

int wlan_wpa_init(void)
{
    int fd, length;
    char *buffer, *offset_ptr;
    struct stat s;

	if (_ctx != RT_NULL) return 0;

    if (stat(WPA_BIN_FILE, &s) !=0)
    {
        rt_kprintf("Access %s failed\n", WPA_BIN_FILE);
        return -1;
    }

    fd = open(WPA_BIN_FILE, O_RDONLY, 0);
    if (fd < 0)
    {
		/* open failed, return */
		rt_kprintf("Open WPA failed.\n");
        return -1;
    }

	length = s.st_size;

	// buffer = (char*)rt_malloc (length);
	buffer = (char*) 0x10000000;
	if (buffer == RT_NULL)
	{
		close(fd);
		return -1;
	}

	offset_ptr = buffer;
    do
    {
        length = read(fd, offset_ptr, 4096);
        if (length > 0)
        {
            offset_ptr += length;
        }
    }while (length > 0);

    /* close fd */
    close(fd);

    rt_module_load("wpa", (void *)buffer);
    // rt_free(buffer);

    return 0;
}

int wlan_wpa_done(void)
{
    int result;

    if (_ctx != RT_NULL)
    {
        result = _ctx->done();
        if (result == 0)
        {
            _ctx = RT_NULL;
        }
    }
	
	return 0;
}

