/*
 * �����嵥��view��ʾ
 *
 * ����һ����ͼ����ʾ��Ҳ��Ϊ���������GUI��ʾ����������ͼ������˵������������ؼ�����ʾ
 * ���ǲ��ã��ȴ���һ��demo_view����ʾ��ͼ����Ȼ�����������ʾ��ͼ�������Ӧ�Ŀؼ���
 *
 * �����ʾ��ͼĬ���Ϸ���һ����ʾ���⣬�·���������ť��������л���ǰһ����ͼ���һ����ͼ��
 * ��Կؼ���ʾ���ԣ������ʾ��ͼ����Ҫ�����ṩ��һ������ʾ������ֻ��Ҫ��������������
 * �ؼ����ɴﵽ��ʾ��Ŀ�ġ�
 *
 * ��������ʾ����ĺ����ǣ�
 * demo_view_get_rect������
 */
#ifndef __DEMO_VIEW_H__
#define __DEMO_VIEW_H__

#include <rtgui/rtgui.h>
#include <rtgui/rtgui_app.h>
#include <rtgui/widgets/container.h>

/* ����Ǳ�׼�汾����������box�Զ��������� */
#ifndef RTGUI_USING_SMALL_SIZE
#include <rtgui/widgets/box.h>
#endif

/* ����һ����ʾ��ͼ����Ҫ���������ͼ����ʾ���� */
rtgui_container_t *demo_view(const char *title);
/* �����ʾ��ͼ�ṩ����ʾ�ؼ��õ�������Ϣ */
void demo_view_get_rect(rtgui_container_t *view, rtgui_rect_t *rect);
void demo_view_get_logic_rect(rtgui_container_t *view, rtgui_rect_t *rect);
void demo_view_show(void);

#endif

