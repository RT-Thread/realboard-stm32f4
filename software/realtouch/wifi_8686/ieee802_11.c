#include <rtthread.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>

#include <wlan_wpa.h>

#define ETHYPT_EAP 0x888e

rt_bool_t is_ieee802_11(struct pbuf* p)
{
	struct eth_hdr* ethhdr;
	rt_uint8_t hdbuf[6];
	rt_uint16_t type;
	rt_uint8_t *payload;

	ethhdr = p->payload;
	type = htons(ethhdr->type);

	if (type == ETHYPT_EAP)
	{
		rt_uint8_t* ptr;
		rt_uint32_t len;

		payload = p->payload;
		rt_memcpy(hdbuf, payload + 6, 6);

		ptr = (rt_uint8_t*) rt_malloc (p->tot_len);
		if (ptr == RT_NULL)
			return RT_FALSE;

		pbuf_header(p, -((rt_int16_t)sizeof(struct eth_hdr)));

		len = p->tot_len;
		pbuf_copy_partial(p, ptr, len, 0);
		pbuf_free(p);
		
		wlan_wpa_eapol_input(ptr, len, hdbuf);

		/* release buffer */
		rt_free(ptr);

		return RT_TRUE;
	}

	return RT_FALSE;
}

