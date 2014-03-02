#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__

#define RT_TOUCH_NORMAL            0
#define RT_TOUCH_CALIBRATION_DATA  1
#define RT_TOUCH_CALIBRATION       2

struct calibration_data
{
    rt_uint16_t min_x, max_x;
    rt_uint16_t min_y, max_y;
};

typedef void (*rt_touch_calibration_func_t)(rt_uint16_t x, rt_uint16_t y);

/** This let the user space to restore the last calibration data.
 *
 * calibration_restore is a callback before calibration started. If it returns
 * RT_TRUE, the calibration won't be started. In this condition, you must setup
 * the calibration_data via something like:
 *
 *     device = rt_device_find("touch");
 *     if(device != RT_NULL)
 *         rt_device_control(device, RT_TOUCH_CALIBRATION_DATA, &data);
 *
 * It it returns RT_FALSE, the normal calibration process will be started. If
 * you don't have such feature, there is no need to call this function. The
 * calibration will always be started by RTGUI.
 */
void calibration_set_restore(rt_bool_t (*calibration_restore)(void));

/** This provide ways to save the calibration_data to user space.
 *
 * calibration_after is a callback after the calibration has finished.  User
 * space could use this function to save the data to some where else. No need
 * to call this if you don't have such function.
 */
void calibration_set_after(void (*calibration_after)(struct calibration_data *data));

void calibration_init(void);

#endif /* end of include guard: __CALIBRATION_H__ */
