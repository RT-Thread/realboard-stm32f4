#include <rtthread.h>

#include "wlan_debug.h"
#include "wlan_types.h"
#include "stdio.h"

unsigned int WlanDebugLevel=WlanErr;
//unsigned int WlanDebugLevel=WlanMsg|WlanCmd|WlanData|WlanErr|WlanEncy|WlanDump;
//unsigned int WlanDebugLevel=WlanMsg|WlanCmd|WlanErr;

/**
 * memmove - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * Unlike memcpy(), memmove() copes with overlapping areas.
 */
static void *memmove(void *dest, const void *src, unsigned int count)
{
	char *tmp;
	const char *s;

	if (dest <= src) {
		tmp = dest;
		s = src;
		while (count--)
			*tmp++ = *s++;
	} else {
		tmp = dest;
		tmp += count;
		s = src;
		s += count;
		while (count--)
			*--tmp = *--s;
	}
	return dest;
}

void hexdump(char *prompt, u8 * buf, int len)
{
	if (WlanDebugLevel & WlanDump)
	{
		int i;
		rt_kprintf("%s", prompt);
		for (i = 0; i < len; i++)
		{
			rt_kprintf("0x%02x ", buf[i]);
		}
		rt_kprintf("\r\n");
	}
}

