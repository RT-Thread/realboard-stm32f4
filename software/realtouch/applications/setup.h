#ifndef __SETUP_H_
#define __SETUP_H_
#include <rtthread.h>

struct setup_items
{
    rt_uint16_t touch_min_x;
    rt_uint16_t touch_max_x;
    rt_uint16_t touch_min_y;
    rt_uint16_t touch_max_y;        
};

rt_err_t setup_load(struct setup_items* setup);
rt_err_t setup_save(struct setup_items* setup);

#endif