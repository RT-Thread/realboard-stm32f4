#ifndef	USER_APP
#define USER_APP
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbconfig.h"
#include "mbframe.h"
#include "mbutils.h"

/* -----------------------Slave Defines -------------------------------------*/
#define S_DISCRETE_INPUT_START        1
#define S_DISCRETE_INPUT_NDISCRETES   16
#define S_COIL_START                  1
#define S_COIL_NCOILS                 64
#define S_REG_INPUT_START             1
#define S_REG_INPUT_NREGS             100
#define S_REG_HOLDING_START           1
#define S_REG_HOLDING_NREGS           100
//�ӻ�ģʽ���ڱ��ּĴ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_HD_RESERVE                     0		  //����
#define          S_HD_CPU_USAGE_MAJOR             1         //��ǰCPU�����ʵ�����λ
#define          S_HD_CPU_USAGE_MINOR             2         //��ǰCPU�����ʵ�С��λ

//�ӻ�ģʽ��������Ĵ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_IN_RESERVE                     0		  //����

//�ӻ�ģʽ������Ȧ�У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_CO_RESERVE                     2		  //����

//�ӻ�ģʽ������ɢ�����У�������ַ��Ӧ�Ĺ��ܶ���
#define          S_DI_RESERVE                     1		  //����

/* -----------------------Master Defines -------------------------------------*/
#define M_DISCRETE_INPUT_START        1
#define M_DISCRETE_INPUT_NDISCRETES   16
#define M_COIL_START                  1
#define M_COIL_NCOILS                 64
#define M_REG_INPUT_START             1
#define M_REG_INPUT_NREGS             100
#define M_REG_HOLDING_START           1
#define M_REG_HOLDING_NREGS           100
//����ģʽ���ڱ��ּĴ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_HD_RESERVE                     0		  //����

//����ģʽ��������Ĵ����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_IN_RESERVE                     0		  //����

//����ģʽ������Ȧ�У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_CO_RESERVE                     2		  //����

//����ģʽ������ɢ�����У�������ַ��Ӧ�Ĺ��ܶ���
#define          M_DI_RESERVE                     1		  //����

#endif
