#include "tetris_ui.h"

/*������ʱ��*/
rtgui_timer_t *timer;
rt_bool_t tobuttom = RT_FALSE;
rt_uint32_t score = 0;
rt_uint8_t level = 1;
rtgui_win_t *win;
const rtgui_color_t tetris_colors[] = {0, RTGUI_RGB(0x00, 0x00, 0xff), RTGUI_RGB(0xff, 0x00, 0x00), RTGUI_RGB(0xff, 0x00, 0xff), RTGUI_RGB(0x00, 0xff, 0x00), RTGUI_RGB(0xff, 0xff, 0x00)}; //��ɫ���飬����Ҫ�õ�����ɫ
const rt_uint8_t box[56] =               //�������飬�������ַ��飬ÿ������״̬
{
    0x80, 0x0e, 0x46, 0x04, 0x70, 0x01, 0x20, 0x62, /*J*/
    0x20, 0x0e, 0x44, 0x06, 0x70, 0x04, 0x60, 0x22, /*L*/
    0x60, 0x0c, 0x64, 0x02, 0x30, 0x06, 0x40, 0x26, /*s*/
    0xc0, 0x06, 0x62, 0x04, 0x60, 0x03, 0x20, 0x46, /*z*/
    0x40, 0x0e, 0x64, 0x04, 0x70, 0x02, 0x20, 0x26, /*T*/
    0x44, 0x44, 0xf0, 0x00, 0x22, 0x22, 0x00, 0x0f, /*I*/
    0xc0, 0x0c, 0x66, 0x00, 0x30, 0x03, 0x00, 0x66, /*O*/
};

tetrisBox now, next;

rt_uint8_t tetris_map[15][25];                   //��ͼ���飬������ʮ��ʮ�У��淽�����ɫ����

void tetris_init_map()
{
    rt_uint8_t i, j;
    for (i = 0; i < 15; i++)
    {
        for (j = 0; j < 25; j++)
        {
            tetris_map[i][j] = 0;
        }
    }

}
void tetris_drawbox(struct rtgui_dc *dc, rt_uint16_t bx, rt_uint16_t by, rtgui_color_t bcolor)
{
    rtgui_rect_t rect = (rtgui_rect_t)
    {
        bx * 16 + XOFFSET,
           by * 16 + YOFFSET,
           bx * 16 + 15 + XOFFSET,
           by * 16 + 15 + YOFFSET
    };
    RTGUI_DC_FC(dc) = RTGUI_RGB(0x00, 0x00, 0xff);
    rtgui_dc_draw_rect(dc, &rect);
    rect.x1++;
    rect.x2--;
    rect.y1++;
    rect.y2--;
    RTGUI_DC_FC(dc) = bcolor;
    rtgui_dc_fill_rect_forecolor(dc, &rect);

}

void tetris_removebox(struct rtgui_dc *dc, rt_uint16_t bx, rt_uint16_t by)
{
    rtgui_rect_t rect = (rtgui_rect_t)
    {
        bx * 16 + XOFFSET,
           by * 16 + YOFFSET,
           bx * 16 + 15 + XOFFSET,
           by * 16 + + 15 + YOFFSET
    };
    RTGUI_DC_FC(dc) = RTGUI_RGB(0x55, 0x55, 0x55);
    rtgui_dc_fill_rect_forecolor(dc, &rect);
}

void tetris_drawTetrisBox(struct rtgui_dc *dc, tetrisBox tbox)
{
    rt_uint8_t i, j, k, l;
    //RTGUI_DC_FC(dc)=tetris_colors[tbox.colorType];
    i = tbox.type * 8 + tbox.state * 2;
    l = box[i];
    for (j = 0; j < 2; j++)
    {
        for (k = 0; k < 4; k++)
        {
            if (l & 0x01)
            {
                tetris_drawbox(dc, tbox.x + k, tbox.y + j, tetris_colors[tbox.colorType]);
                if (tbox.x < 15)
                {
                    tetris_map[tbox.x + k][tbox.y + j] = tbox.colorType;
                }
            }
            l >>= 1;
        }
    }
    l = box[i + 1];
    for (j = 0; j < 2; j++)
    {
        for (k = 0; k < 4; k++)
        {
            if (l & 0x01)
            {
                tetris_drawbox(dc, tbox.x + k, tbox.y + j + 2, tetris_colors[tbox.colorType]);
                if (tbox.x < 15)
                {
                    tetris_map[tbox.x + k][tbox.y + j + 2] = tbox.colorType;
                }
            }
            l >>= 1;
        }
    }
}
void tetris_clearTetrisBox(struct rtgui_dc *dc)
{
    rt_uint8_t i, j, k, l;
    i = now.type * 8 + now.state * 2;
    l = box[i];
    for (j = 0; j < 2; j++)
    {
        for (k = 0; k < 4; k++)
        {
            if (l & 0x01)
            {
                tetris_removebox(dc, now.x + k, now.y + j);
                tetris_map[now.x + k][now.y + j] = 0;
            }
            l >>= 1;
        }
    }
    l = box[i + 1];
    for (j = 0; j < 2; j++)
    {
        for (k = 0; k < 4; k++)
        {
            if (l & 0x01)
            {
                tetris_removebox(dc, now.x + k, now.y + j + 2);
                tetris_map[now.x + k][now.y + j + 2] = 0;
            }
            l >>= 1;
        }
    }
}
void tetris_movedown(struct rtgui_dc *dc)               //���Ʒ���
{
    tetris_clearTetrisBox(dc);
    now.y++;
    if (tetris_checkcrack())
    {
        tobuttom = RT_TRUE;
        now.y--;
        tetris_drawTetrisBox(dc, now);
        tetris_checkmap(dc);
    }
    else
    {
        tetris_drawTetrisBox(dc, now);
    }
}

void tetris_refreshmap(struct rtgui_dc *dc)                               //ˢ�µ�ͼ
{
    rt_uint8_t i, j;
    rt_uint8_t buf[5];
    rtgui_rect_t rect = {XOFFSET, YOFFSET, XOFFSET + 240, YOFFSET + 400};
    RTGUI_DC_FC(dc) = RTGUI_RGB(0x55, 0x55, 0x55);
    rtgui_dc_fill_rect_forecolor(dc, &rect);
    rect.x1 -= 10;
    rect.x2 += 150;
    rect.y1 -= 10;
    rect.y2 += 10;
    RTGUI_DC_FC(dc) = RTGUI_RGB(0x00, 0x00, 0xff);
    rtgui_dc_draw_rect(dc, &rect);
    rect.x1 += 290;
    rect.x2 -= 30;
    rect.y1 += 40;
    rect.y2 -= 360;
    RTGUI_DC_FC(dc) = RTGUI_RGB(0x00, 0x00, 0x00);
    rtgui_dc_draw_text(dc, "��һ��", &rect);
    rect.y1 += 130;
    rect.y2 += 130;
    rtgui_dc_draw_text(dc, "�ܵ÷�", &rect);
    rect.y1 += 80;
    rect.y2 += 80;
    rtgui_dc_draw_text(dc, "LEVEL", &rect);
    RTGUI_DC_FC(dc) = RTGUI_RGB(0xff, 0x00, 0x00);
    rect.x1 += 20;
    rect.x2 += 20;
    rect.y1 -= 60;
    rect.y2 -= 60;
    rt_sprintf(buf, "%d", score);
    rtgui_dc_fill_rect(dc, &rect);
    rtgui_dc_draw_text(dc, buf, &rect);
    rect.y1 += 80;
    rect.y2 += 80;
    rt_sprintf(buf, "%d", level);
    rtgui_dc_fill_rect(dc, &rect);
    rtgui_dc_draw_text(dc, buf, &rect);


    for (i = 0; i < 25; i++)
    {
        for (j = 0; j < 15; j++)
        {
            if (tetris_map[j][i] != 0)
            {
                tetris_drawbox(dc, j, i, tetris_colors[tetris_map[j][i]]);
            }
        }
    }
}
rt_bool_t tetris_checkcrack()
{
    rt_bool_t isCrack = RT_FALSE;
    rt_uint8_t i, j, k, l;
    rt_int16_t x, y;
    i = now.type * 8 + now.state * 2;
    l = box[i];
    for (j = 0; j < 2; j++)
    {
        for (k = 0; k < 4; k++)
        {
            if (l & 0x01)
            {
                x = now.x + k;
                y = now.y + j;
                if (x < 0 || x > 14 || y > 24 || tetris_map[x][y] != 0) // ����Խ���߽�����Ѿ���ռ��Ϊ����ײ
                {
                    isCrack = RT_TRUE;
                    break;
                }
            }
            l >>= 1;
        }
    }
    if (!isCrack)
    {
        l = box[i + 1];
        for (j = 0; j < 2; j++)
        {
            for (k = 0; k < 4; k++)
            {
                if (l & 0x01)
                {
                    x = now.x + k;
                    y = now.y + j + 2;
                    if (x < 0 || x > 14 || y > 24 || tetris_map[x][y] != 0)
                    {
                        isCrack = RT_TRUE;
                        break;
                    }
                }
                l >>= 1;
            }
        }
    }
    return isCrack;
}
void tetris_checkmap(struct rtgui_dc *dc)            //����Ƿ���Ҫ����
{
    rt_int8_t i, j, k;
    rt_uint8_t cnt = 0;
    for (i = 24; i >= 0; i--)
    {
        k = 0;
        for (j = 0; j < 15; j++)
        {
            if (tetris_map[j][i] != 0)
            {
                k++;
            }
        }
        if (k == 15)                 //���һ����ȫ����ռλ��Ϊ��Ҫ����
        {
            cnt++;
            for (j = i; j > 0; j--)
            {
                for (k = 0; k < 15; k++)
                {
                    tetris_map[k][j] = tetris_map[k][j - 1];
                }
            }
            for (k = 0; k < 15; k++)
            {
                tetris_map[k][0] = 0;
            }
            i++;
            tetris_refreshmap(dc);
        }
    }
    score += cnt * cnt;
    if (score >= level * 100)
    {
        level++;
    }
    tetris_refreshmap(dc);
}
void tetris_clearnext(struct rtgui_dc *dc)                            //�����һ������
{
    rt_uint8_t x, y;
    for (x = 17; x < 21; x++)
    {
        for (y = 3; y < 7; y++)
        {
            tetris_removebox(dc, x, y);
        }
    }
}

void tetris_createNext(struct rtgui_dc *dc)                            //������һ������
{
    tetris_clearnext(dc);
    next.x = 17;
    next.y = 3;
    next.colorType = rand() % 5 + 1;      //�����ɫ
    next.type = rand() % 7;               //�������
    next.state = rand() % 4;               //���״̬
    tetris_drawTetrisBox(dc, next);
}
void tetris_new_into()
{
    now = next;
    now.x = 5;
    now.y = 0;
}
/**
  * @�������ƣ�event_handler()
  * @����    ��  �������¼�������
  * @����    ��  *object: �¼�����ָ��,*event:�¼�����ָ��
  * @����ֵ  �� rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    /*���������Ϣ*/
    rt_kprintf("event_handler\n");

    if (event->type == RTGUI_EVENT_PAINT)        //�¼�����ΪRTGUI_EVENT_PAINT
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_PAINT\n");
        /*ִ�д����¼�*/
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
        saved = RTGUI_DC_FC(dc);
        tetris_refreshmap(dc);
        tetris_createNext(dc);
        tetris_new_into();
        tetris_createNext(dc);
        RTGUI_DC_FC(dc) = saved;
        rtgui_dc_end_drawing(dc);
        rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //�¼�����ΪRTGUI_EVENT_SHOW
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_SHOW\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        //rtgui_timer_start(timer);
    }
    else if (event->type == RTGUI_EVENT_HIDE)  //�¼�����ΪRTGUI_EVENT_HIDE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_HIDE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //�¼�����ΪRTGUI_EVENT_WIN_DEACTIVATE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        rtgui_timer_stop(timer);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //�¼�����ΪRTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        rtgui_timer_stop(timer);
        rtgui_dc_end_drawing(dc);
    }
    else                                       //���ڷ�Χ�ڵ��¼�
    {
        rt_kprintf("event->type:%d\n", event->type);
        return rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    return RT_FALSE;
}
/**
  * @�������ƣ�timeout()
  * @����    ��  �������¼�������
  * @����    ��  struct rtgui_timer *timer,void *parameter
  * @����ֵ  �� void.
  */

static void timeout(struct rtgui_timer *timer, void *parameter)
{
    struct rtgui_widget *widget;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    rtgui_color_t saved;
    saved = RTGUI_DC_FC(dc);
    tetris_movedown(dc);
    if (tobuttom)
    {
        //rt_kprintf("tobuttom\r\n");
        tobuttom = RT_FALSE;
        tetris_new_into();
        tetris_createNext(dc);
    }
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
    // rt_kprintf("into timeout\r\n");

}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void up_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    now.state = (++now.state) % 4;
    if (tetris_checkcrack())
    {
        now.state = (--now.state) % 4;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void down_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    now.y++;
    if (tetris_checkcrack())
    {
        now.y--;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void left_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    now.x--;
    if (tetris_checkcrack())
    {
        now.x++;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void right_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    now.x++;
    if (tetris_checkcrack())
    {
        now.x--;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void turnl_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    if (now.state == 0)
    {
        now.state = 3;
    }
    else
    {
        now.state--;
    }
    if (tetris_checkcrack())
    {
        now.state = (++now.state) % 4;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
/**
* @�������ƣ�button1_pressed
* @��   �� ��button1�¼�����
* @��   �� ��*object rtgui����ָ�룬*event�¼�ָ��
* @����ֵ  ��void
**/
void turnr_button_pressed(struct rtgui_object *object, struct rtgui_event *event)
{
    rtgui_color_t saved;
    struct rtgui_dc *dc;
    dc = rtgui_dc_begin_drawing(RTGUI_WIDGET(win));
    saved = RTGUI_DC_FC(dc);
    tetris_clearTetrisBox(dc);
    now.state = (++now.state) % 4;
    if (tetris_checkcrack())
    {
        now.state = (--now.state) % 4;
    }
    tetris_drawTetrisBox(dc, now);
    RTGUI_DC_FC(dc) = saved;
    rtgui_dc_end_drawing(dc);
}
void tetris_ui_init(struct rtgui_app *app)
{

    // rtgui_win_t *win;
    rtgui_rect_t rect;
    rtgui_button_t *up_button, *down_button, *left_button, *right_button, *turnl_button, *turnr_button;
    rtgui_image_t *image1, *image2;
    /*��ʼ����ͼ*/
    tetris_init_map();
    /*����������*/
    win = rtgui_mainwin_create(RT_NULL, "tetris",
                               RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    /*�ж��Ƿ�ɹ�����������*/
    if (RT_NULL == win)
    {
        rt_kprintf("create main window failed!\n");
        rtgui_app_destroy(app);
        return;
    }
    RTGUI_WIDGET_BACKGROUND(win) = RTGUI_RGB(0xff, 0xff, 0xff);
    RTGUI_WIDGET_FONT(win) = rtgui_font_refer("hz", 16);
    /*��������¼�*/
    rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*������ť*/
    up_button = rtgui_button_create("U");
    down_button = rtgui_button_create("D");
    left_button = rtgui_button_create("L");
    right_button = rtgui_button_create("R");
    turnl_button = rtgui_button_create("TL");
    turnr_button = rtgui_button_create("TR");
    /*���ð�ťλ��*/
    rect = (rtgui_rect_t)
    {
        30, 250, 78, 298
    };
    rtgui_widget_set_rect(RTGUI_WIDGET(left_button), &rect);
    rect.x1 += 120;
    rect.x2 += 120;
    rtgui_widget_set_rect(RTGUI_WIDGET(right_button), &rect);
    rect.x1 -= 60;
    rect.x2 -= 60;
    rect.y1 -= 60;
    rect.y2 -= 60;
    rtgui_widget_set_rect(RTGUI_WIDGET(up_button), &rect);
    rect.y1 += 120;
    rect.y2 += 120;
    rtgui_widget_set_rect(RTGUI_WIDGET(down_button), &rect);
    rect.x1 += 600;
    rect.x2 += 600;
    rtgui_widget_set_rect(RTGUI_WIDGET(turnr_button), &rect);
    rect.y1 -= 120;
    rect.y2 -= 120;
    rtgui_widget_set_rect(RTGUI_WIDGET(turnl_button), &rect);
    image1 = rtgui_image_create(RESOURCE_PATH"tr1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(turnr_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"tr2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(turnr_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"tl1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(turnl_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"tl2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(turnl_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"u1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(up_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"u2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(up_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"d1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(down_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"d2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(down_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"l1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(left_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"l2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(left_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"r1.bmp", RT_FALSE);
    rtgui_button_set_unpressed_image(right_button, image1);
    image1 = rtgui_image_create(RESOURCE_PATH"r2.bmp", RT_FALSE);
    rtgui_button_set_pressed_image(right_button, image1);

    /*��Ӱ�ť*/
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(up_button));
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(left_button));
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(down_button));
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(right_button));
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(turnl_button));
    rtgui_container_add_child(RTGUI_CONTAINER(win), RTGUI_WIDGET(turnr_button));


    /*���ð����¼�����*/
    rtgui_button_set_onbutton(up_button, up_button_pressed);
    rtgui_button_set_onbutton(down_button, down_button_pressed);
    rtgui_button_set_onbutton(left_button, left_button_pressed);
    rtgui_button_set_onbutton(right_button, right_button_pressed);
    rtgui_button_set_onbutton(turnl_button, turnl_button_pressed);
    rtgui_button_set_onbutton(turnr_button, turnr_button_pressed);

    /*������ʱ������ʱ��*/
    timer = rtgui_timer_create(RT_TICK_PER_SECOND / level, RT_TIMER_FLAG_PERIODIC, timeout, (void *)win);
    /*��ʾ������*/
    rtgui_win_show(win, RT_TRUE);

}