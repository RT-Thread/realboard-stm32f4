#include <rtthread.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>

void gui_init(void)
{
	rt_device_t device;
	struct rt_device_rect_info info;

	device = rt_device_find("lcd");
	/* re-set graphic device */
	rtgui_graphic_set_device(device);
}
