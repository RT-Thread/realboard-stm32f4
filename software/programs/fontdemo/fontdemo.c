#include <rtthread.h>
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>
#include <rtgui/widgets/window.h>
#include <rtgui/dc.h>
#include <rtgui/font.h>


struct rtgui_hz_file_font hz24 =
{
    {RT_NULL},              /* cache root       */
    0,                      /* cache size       */
    24,                     /* font size        */
    72,                     /* font data size   */
    -1,                     /* fd               */
    "/SD/resource/hzk24.fnt"    /* font_fn          */
};

struct rtgui_font rtgui_font_hz24 =
{
    "hz",                   /* family */
    24,                     /* height */
    1,                      /* refer count */
    &rtgui_hz_file_font_engine,/* font engine */
    (void *) &hz24,         /* font private data */
};


/*rtgui_dc���ƻ���ͼ�κ���*/
static void dc_draw(struct rtgui_widget *widget);
/*�������¼�����*/
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event);


/**
  * @�������ƣ�event_handler()
  * @����    ��  �������¼�������
  * @����    ��  *object: �¼�����ָ��,*event:�¼�����ָ��
  * @����ֵ  �� rt_bool_t.
  */
static rt_bool_t event_handler(struct rtgui_object *object, rtgui_event_t *event)
{
    /*��objectת����widget*/
    struct rtgui_widget *widget = RTGUI_WIDGET(object);
    /*���������Ϣ*/
    rt_kprintf("event_handler\n");

    if (event->type == RTGUI_EVENT_PAINT)        //�¼�����ΪRTGUI_EVENT_PAINT
                     {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_PAINT\n");
        /*ִ�д����¼�*/
        rtgui_win_event_handler((struct rtgui_object *)object, event);
        /*���û��ƺ�������ͼ��*/
        dc_draw(widget);
    }
    else if (event->type == RTGUI_EVENT_SHOW)    //�¼�����ΪRTGUI_EVENT_SHOW
               {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_SHOW\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);

    }
    else if (event->type == RTGUI_EVENT_HIDE)  //�¼�����ΪRTGUI_EVENT_HIDE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_HIDE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else if (event->type == RTGUI_EVENT_WIN_DEACTIVATE) //�¼�����ΪRTGUI_EVENT_WIN_DEACTIVATE
    {
        /*���������Ϣ*/
        rt_kprintf("RTGUI_EVENT_WIN_DEACTIVATE\n");
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else if (event->type == RTGUI_EVENT_KBD)   //�¼�����ΪRTGUI_EVENT_KBD
    {
        rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    else                                       //���ڷ�Χ�ڵ��¼�
    {
        rt_kprintf("event->type:%d\n", event->type);
        return rtgui_win_event_handler((struct rtgui_object *)object, event);
    }
    return RT_FALSE;
}

/**
  * @�������ƣ�  dc_draw()
  * @����    ��  dc��ͼ�����������ӵĺ���
  * @����    ��  *widget RTGUi����ָ��
  * @����ֵ  ��  None.
  */
static void dc_draw(struct rtgui_widget *widget)
{
    /*�ֲ�����*/
    struct rtgui_dc *dc;
    rtgui_color_t tcolor ;
    rtgui_rect_t rect = {30, 80, 320, 110};
    /*��ô����dc*/
    dc = rtgui_dc_begin_drawing(widget);
    /*��ŵ�ǰDC��ǰ��ɫ*/
    tcolor = RTGUI_DC_FC(dc);
    /*����ǰ��ɫ*/
    RTGUI_DC_FC(dc) = RTGUI_RGB(0, 0, 200);
    /*����ʹ�õ�����Ϊasc16*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("asc", 16);
    /*������ʾ����*/
    rtgui_dc_draw_text(dc, "ASC16:hello world!", &rect);
    /*����ʹ�õ�����Ϊ16x16����*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("hz", 16);
    /*���û��Ƶ�����*/
    rect = (rtgui_rect_t)
    {
        30, 130, 320, 160
    };
    /*����16X16��ʾ����*/
    rtgui_dc_draw_text(dc, "HZK16:��ã�����16x16����!", &rect);
    /*����ʹ������Ϊ24x24����*/
    RTGUI_WIDGET_FONT(widget) = rtgui_font_refer("hz", 24);
    /*���û��Ƶ�����*/
    rect = (rtgui_rect_t)
    {
        30, 180, 320, 210
    };
    /*����24x24��ʾ����*/
    rtgui_dc_draw_text(dc, "HZK24:���,����24x24����!", &rect);
    /*���û��Ƶ�����*/
    rect = (rtgui_rect_t)
    {
        30, 10, 320, 40
    };
    /*���Ʊ���*/
    rtgui_dc_draw_text(dc, "����һ���ֿ�ʹ������", &rect);
    /*��ԭ����ǰ��ɫ���û�ȥ*/
    RTGUI_DC_FC(dc) = tcolor;
    /*����dc��ͼ*/
    rtgui_dc_end_drawing(dc);

}

void main(void)
{
    /*�ֲ�����*/
    struct rtgui_app *application;
    rtgui_win_t *win;
    /*����Ӧ�ó���*/
    application = rtgui_app_create("fontdemo");
    /*�ж��Ƿ�ɹ�����Ӧ�ó���*/
    if (RT_NULL == application)
    {
        rt_kprintf("create application \"fontdemo\" failed!\n");
        return ;
    }
    /*����������*/
    win = rtgui_mainwin_create(RT_NULL, "fontdemo",
                               RTGUI_WIN_STYLE_MAINWIN | RTGUI_WIN_STYLE_DESTROY_ON_CLOSE);
    /*�ж��Ƿ�ɹ�����������*/
    if (RT_NULL == win)
    {
        rt_kprintf("create main window failed!\n");
        rtgui_app_destroy(application);
        return;
    }
    /*�����ⲿ����*/
    rtgui_font_system_add_font(&rtgui_font_hz24);

    /*��������¼�*/
    rtgui_object_set_event_handler(RTGUI_OBJECT(win), event_handler);
    /*��ʾ������*/
    rtgui_win_show(win, RT_TRUE);
    rtgui_app_run(application);
    rtgui_app_destroy(application);
}
