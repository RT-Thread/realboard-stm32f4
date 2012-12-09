#ifndef __TETRIS_UI_H__
#define __TETRIS_UI_H__

#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>
#include <rtgui/widgets/button.h>
#include <rtgui/image.h>


#define XOFFSET  240
#define YOFFSET  15

#define RESOURCE_PATH "/SD/programs/tetris/resource/"

typedef struct                           //����ṹ�壬�������ͣ�״̬�����꣬��ɫ
{
    rt_uint8_t type;                        //��������ͣ�LJITOSZ��
    rt_uint8_t state;                       //�����״̬��0��90��180��270����ת��
    rt_int16_t x, y;                       //���������
    rt_uint8_t colorType;                   //��ɫ����
} tetrisBox;


void tetris_ui_init(struct rtgui_app *app);
rt_bool_t tetris_checkcrack();
void tetris_refreshmap(struct rtgui_dc *dc);
void tetris_movedown(struct rtgui_dc *dc);
void tetris_clearTetrisBox(struct rtgui_dc *dc);
void tetris_drawTetrisBox(struct rtgui_dc *dc, tetrisBox tbox);
void tetris_removebox(struct rtgui_dc *dc, rt_uint16_t bx, rt_uint16_t by);
void tetris_drawbox(struct rtgui_dc *dc, rt_uint16_t bx, rt_uint16_t by, rtgui_color_t bcolor);
void tetris_checkmap();
void tetris_clearnext(struct rtgui_dc *dc);
void tetris_createNext();


#endif