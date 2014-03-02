#ifndef __SEP4020_H
#define __SEP4020_H

#include <rtthread.h>

/*Core definations*/
#define SVCMODE
#define	Mode_USR	0x10
#define	Mode_FIQ	0x11
#define	Mode_IRQ	0x12
#define	Mode_SVC	0x13
#define	Mode_ABT	0x17
#define	Mode_UND	0x1B
#define	Mode_SYS	0x1F



/*
 *  ��ģ��Ĵ�����ֵ
 */

#define   ESRAM_BASE    0x04000000
#define   INTC_BASE     0x10000000
#define   PMU_BASE      0x10001000
#define   RTC_BASE      0x10002000
#define   WD_BASE       0x10002000
#define   TIMER_BASE    0x10003000
#define   PWM_BASE      0x10004000
#define   UART0_BASE    0X10005000
#define   UART1_BASE    0X10006000
#define   UART2_BASE    0X10007000
#define   UART3_BASE    0X10008000
#define   SSI_BASE      0X10009000
#define   I2S_BASE      0x1000A000
#define   MMC_BASE      0x1000B000
#define   SD_BASE       0x1000B000
#define   SMC0_BASE     0x1000C000
#define   SMC1_BASE     0x1000D000
#define   USBD_BASE     0x1000E000
#define   GPIO_BASE     0x1000F000
#define   EMI_BASE      0x11000000
#define   DMAC_BASE     0x11001000
#define   LCDC_BASE     0x11002000
#define   MAC_BASE      0x11003000
#define   AMBA_BASE     0x11005000


/*
 *  INTCģ��
 *  ��ַ: 0x10000000
 */

#define INTC_IER                  (INTC_BASE+0X000)       /* IRQ�ж�����Ĵ��� */
#define INTC_IMR                  (INTC_BASE+0X008)       /* IRQ�ж����μĴ��� */
#define INTC_IFR                  (INTC_BASE+0X010)       /* IRQ���ǿ���жϼĴ��� */
#define INTC_IRSR                 (INTC_BASE+0X018)       /* IRQδ�����ж�״̬�Ĵ��� */
#define INTC_ISR                  (INTC_BASE+0X020)       /* IRQ�ж�״̬�Ĵ��� */
#define INTC_IMSR                 (INTC_BASE+0X028)       /* IRQ�����ж�״̬�Ĵ��� */
#define INTC_IFSR                 (INTC_BASE+0X030)       /* IRQ�ж�����״̬�Ĵ��� */
#define INTC_FIER                 (INTC_BASE+0X0C0)       /* FIQ�ж�����Ĵ��� */
#define INTC_FIMR                 (INTC_BASE+0X0C4)       /* FIQ�ж����μĴ��� */
#define INTC_FIFR                 (INTC_BASE+0X0C8)       /* FIQ���ǿ���жϼĴ��� */
#define INTC_FIRSR                (INTC_BASE+0X0CC)       /* FIQδ�����ж�״̬�Ĵ��� */
#define INTC_FISR                 (INTC_BASE+0X0D0)       /* FIQ�ж�״̬�Ĵ��� */
#define INTC_FIFSR                (INTC_BASE+0X0D4)       /* FIQ�ж�����״̬�Ĵ��� */
#define INTC_IPLR                 (INTC_BASE+0X0D8)       /* IRQ�ж����ȼ��Ĵ��� */
#define INTC_ICR1                 (INTC_BASE+0X0DC)       /* IRQ�ڲ��ж����ȼ����ƼĴ���1 */
#define INTC_ICR2                 (INTC_BASE+0X0E0)       /* IRQ�ڲ��ж����ȼ����ƼĴ���2 */
#define INTC_EXICR1               (INTC_BASE+0X0E4)       /* IRQ�ⲿ�ж����ȼ����ƼĴ���1 */
#define INTC_EXICR2               (INTC_BASE+0X0E8)       /* IRQ�ⲿ�ж����ȼ����ƼĴ���2 */


/*
 *  PMUģ��
 *  ��ַ: 0x10001000
 */

#define PMU_PLTR                  (PMU_BASE+0X000)        /* PLL���ȶ�����ʱ�� */
#define PMU_PMCR                  (PMU_BASE+0X004)        /* ϵͳ��ʱ��PLL�Ŀ��ƼĴ��� */
#define PMU_PUCR                  (PMU_BASE+0X008)        /* USBʱ��PLL�Ŀ��ƼĴ��� */
#define PMU_PCSR                  (PMU_BASE+0X00C)        /* �ڲ�ģ��ʱ��Դ�����Ŀ��ƼĴ��� */
#define PMU_PDSLOW                (PMU_BASE+0X010)        /* SLOW״̬��ʱ�ӵķ�Ƶ���� */
#define PMU_PMDR                  (PMU_BASE+0X014)        /* оƬ����ģʽ�Ĵ��� */
#define PMU_RCTR                  (PMU_BASE+0X018)        /* Reset���ƼĴ��� */
#define PMU_CLRWAKUP              (PMU_BASE+0X01C)        /* WakeUp����Ĵ��� */


/*
 *  RTCģ��
 *  ��ַ: 0x10002000
 */

#define RTC_STA_YMD               (RTC_BASE+0X000)        /* ��, ��, �ռ����Ĵ��� */
#define RTC_STA_HMS               (RTC_BASE+0X004)        /* Сʱ, ����, ��Ĵ��� */
#define RTC_ALARM_ALL             (RTC_BASE+0X008)        /* ��ʱ��, ��, ʱ, �ּĴ��� */
#define RTC_CTR                   (RTC_BASE+0X00C)        /* ���ƼĴ��� */
#define RTC_INT_EN                (RTC_BASE+0X010)        /* �ж�ʹ�ܼĴ��� */
#define RTC_INT_STS               (RTC_BASE+0X014)        /* �ж�״̬�Ĵ��� */
#define RTC_SAMP                  (RTC_BASE+0X018)        /* �������ڼĴ��� */
#define RTC_WD_CNT                (RTC_BASE+0X01C)        /* Watch-Dog����ֵ�Ĵ��� */
#define RTC_WD_SEV                (RTC_BASE+0X020)        /* Watch-Dog����Ĵ��� */ 
#define RTC_CONFIG_CHECK          (RTC_BASE+0X024)        /* ����ʱ��ȷ�ϼĴ��� (������ʱ��֮ǰ��д0xaaaaaaaa) */
#define RTC_KEY0                  (RTC_BASE+0X02C)        /* ��Կ�Ĵ��� */

/*
 *  TIMERģ��
 *  ��ַ: 0x10003000
 */

#define TIMER_T1LCR               (TIMER_BASE+0X000)      /* ͨ��1���ؼ����Ĵ��� */
#define TIMER_T1CCR               (TIMER_BASE+0X004)      /* ͨ��1��ǰ����ֵ�Ĵ��� */
#define TIMER_T1CR                (TIMER_BASE+0X008)      /* ͨ��1���ƼĴ��� */
#define TIMER_T1ISCR              (TIMER_BASE+0X00C)      /* ͨ��1�ж�״̬����Ĵ��� */
#define TIMER_T1IMSR              (TIMER_BASE+0X010)      /* ͨ��1�ж�����״̬�Ĵ��� */
#define TIMER_T2LCR               (TIMER_BASE+0X020)      /* ͨ��2���ؼ����Ĵ��� */
#define TIMER_T2CCR               (TIMER_BASE+0X024)      /* ͨ��2��ǰ����ֵ�Ĵ��� */
#define TIMER_T2CR                (TIMER_BASE+0X028)      /* ͨ��2���ƼĴ��� */
#define TIMER_T2ISCR              (TIMER_BASE+0X02C)      /* ͨ��2�ж�״̬����Ĵ��� */
#define TIMER_T2IMSR              (TIMER_BASE+0X030)      /* ͨ��2�ж�����״̬�Ĵ��� */
#define TIMER_T3LCR               (TIMER_BASE+0X040)      /* ͨ��3���ؼ����Ĵ��� */
#define TIMER_T3CCR               (TIMER_BASE+0X044)      /* ͨ��3��ǰ����ֵ�Ĵ��� */
#define TIMER_T3CR                (TIMER_BASE+0X048)      /* ͨ��3���ƼĴ��� */
#define TIMER_T3ISCR              (TIMER_BASE+0X04C)      /* ͨ��3�ж�״̬����Ĵ��� */
#define TIMER_T3IMSR              (TIMER_BASE+0X050)      /* ͨ��3�ж�����״̬�Ĵ��� */
#define TIMER_T3CAPR              (TIMER_BASE+0X054)      /* ͨ��3����Ĵ��� */
#define TIMER_T4LCR               (TIMER_BASE+0X060)      /* ͨ��4���ؼ����Ĵ��� */
#define TIMER_T4CCR               (TIMER_BASE+0X064)      /* ͨ��4��ǰ����ֵ�Ĵ��� */
#define TIMER_T4CR                (TIMER_BASE+0X068)      /* ͨ��4���ƼĴ��� */
#define TIMER_T4ISCR              (TIMER_BASE+0X06C)      /* ͨ��4�ж�״̬����Ĵ��� */
#define TIMER_T4IMSR              (TIMER_BASE+0X070)      /* ͨ��4�ж�����״̬�Ĵ��� */
#define TIMER_T4CAPR              (TIMER_BASE+0X074)      /* ͨ��4����Ĵ��� */
#define TIMER_T5LCR               (TIMER_BASE+0X080)      /* ͨ��5���ؼ����Ĵ��� */
#define TIMER_T5CCR               (TIMER_BASE+0X084)      /* ͨ��5��ǰ����ֵ�Ĵ��� */
#define TIMER_T5CR                (TIMER_BASE+0X088)      /* ͨ��5���ƼĴ��� */
#define TIMER_T5ISCR              (TIMER_BASE+0X08C)      /* ͨ��5�ж�״̬����Ĵ��� */
#define TIMER_T5IMSR              (TIMER_BASE+0X090)      /* ͨ��5�ж�����״̬�Ĵ��� */
#define TIMER_T5CAPR              (TIMER_BASE+0X094)      /* ͨ��5����Ĵ��� */
#define TIMER_T6LCR               (TIMER_BASE+0X0A0)      /* ͨ��6���ؼ����Ĵ��� */
#define TIMER_T6CCR               (TIMER_BASE+0X0A4)      /* ͨ��6��ǰ����ֵ�Ĵ��� */
#define TIMER_T6CR                (TIMER_BASE+0X0A8)      /* ͨ��6���ƼĴ��� */
#define TIMER_T6ISCR              (TIMER_BASE+0X0AC)      /* ͨ��6�ж�״̬����Ĵ��� */
#define TIMER_T6IMSR              (TIMER_BASE+0X0B0)      /* ͨ��6�ж�����״̬�Ĵ��� */
#define TIMER_T6CAPR              (TIMER_BASE+0X0B4)      /* ͨ��6����Ĵ��� */
#define TIMER_T7LCR               (TIMER_BASE+0X0C0)      /* ͨ��7���ؼ����Ĵ��� */
#define TIMER_T7CCR               (TIMER_BASE+0X0C4)      /* ͨ��7��ǰ����ֵ�Ĵ��� */
#define TIMER_T7CR                (TIMER_BASE+0X0C8)      /* ͨ��7���ƼĴ��� */
#define TIMER_T7ISCR              (TIMER_BASE+0X0CC)      /* ͨ��7�ж�״̬����Ĵ��� */
#define TIMER_T7IMSR              (TIMER_BASE+0X0D0)      /* ͨ��7�ж�����״̬�Ĵ��� */
#define TIMER_T8LCR               (TIMER_BASE+0X0E0)      /* ͨ��8���ؼ����Ĵ��� */
#define TIMER_T8CCR               (TIMER_BASE+0X0E4)      /* ͨ��8��ǰ����ֵ�Ĵ��� */
#define TIMER_T8CR                (TIMER_BASE+0X0E8)      /* ͨ��8���ƼĴ��� */
#define TIMER_T8ISCR              (TIMER_BASE+0X0EC)      /* ͨ��8�ж�״̬����Ĵ��� */
#define TIMER_T8IMSR              (TIMER_BASE+0X0F0)      /* ͨ��8�ж�����״̬�Ĵ��� */
#define TIMER_T9LCR               (TIMER_BASE+0X100)      /* ͨ��9���ؼ����Ĵ��� */
#define TIMER_T9CCR               (TIMER_BASE+0X104)      /* ͨ��9��ǰ����ֵ�Ĵ��� */
#define TIMER_T9CR                (TIMER_BASE+0X108)      /* ͨ��9���ƼĴ��� */
#define TIMER_T9ISCR              (TIMER_BASE+0X10C)      /* ͨ��9�ж�״̬����Ĵ��� */
#define TIMER_T9IMSR              (TIMER_BASE+0X110)      /* ͨ��9�ж�����״̬�Ĵ��� */
#define TIMER_T10LCR              (TIMER_BASE+0X120)      /* ͨ��10���ؼ����Ĵ��� */
#define TIMER_T10CCR              (TIMER_BASE+0X124)      /* ͨ��10��ǰ����ֵ�Ĵ��� */
#define TIMER_T10CR               (TIMER_BASE+0X128)      /* ͨ��10���ƼĴ��� */
#define TIMER_T10ISCR             (TIMER_BASE+0X12C)      /* ͨ��10�ж�״̬����Ĵ��� */
#define TIMER_T10IMSR             (TIMER_BASE+0X130)      /* ͨ��10�ж�����״̬�Ĵ��� */
#define TIMER_TIMSR               (TIMER_BASE+0X140)      /* TIMER�ж�����״̬�Ĵ��� */
#define TIMER_TISCR               (TIMER_BASE+0X144)      /* TIMER�ж�״̬����Ĵ��� */
#define TIMER_TISR                (TIMER_BASE+0X148)      /* TIMER�ж�״̬�Ĵ��� */



/*
 *  PWMģ��
 *  ��ַ: 0x10004000
 */

#define PWM0_CTRL                 (PWM_BASE+0X000)        /* PWM0���ƼĴ��� */
#define PWM0_DIV                  (PWM_BASE+0X004)        /* PWM0��Ƶ�Ĵ��� */
#define PWM0_PERIOD               (PWM_BASE+0X008)        /* PWM0���ڼĴ��� */
#define PWM0_DATA                 (PWM_BASE+0X00C)        /* PWM0���ݼĴ��� */
#define PWM0_CNT                  (PWM_BASE+0X010)        /* PWM0�����Ĵ��� */
#define PWM0_STATUS               (PWM_BASE+0X014)        /* PWM0״̬�Ĵ��� */
#define PWM1_CTRL                 (PWM_BASE+0X020)        /* PWM1���ƼĴ��� */
#define PWM1_DIV                  (PWM_BASE+0X024)        /* PWM1��Ƶ�Ĵ��� */
#define PWM1_PERIOD               (PWM_BASE+0X028)        /* PWM1���ڼĴ��� */
#define PWM1_DATA                 (PWM_BASE+0X02C)        /* PWM1���ݼĴ��� */
#define PWM1_CNT                  (PWM_BASE+0X030)        /* PWM1�����Ĵ��� */
#define PWM1_STATUS               (PWM_BASE+0X034)        /* PWM1״̬�Ĵ��� */
#define PWM2_CTRL                 (PWM_BASE+0X040)        /* PWM2���ƼĴ��� */
#define PWM2_DIV                  (PWM_BASE+0X044)        /* PWM2��Ƶ�Ĵ��� */
#define PWM2_PERIOD               (PWM_BASE+0X048)        /* PWM2���ڼĴ��� */
#define PWM2_DATA                 (PWM_BASE+0X04C)        /* PWM2���ݼĴ��� */
#define PWM2_CNT                  (PWM_BASE+0X050)        /* PWM2�����Ĵ��� */
#define PWM2_STATUS               (PWM_BASE+0X054)        /* PWM2״̬�Ĵ��� */
#define PWM3_CTRL                 (PWM_BASE+0X060)        /* PWM3���ƼĴ��� */
#define PWM3_DIV                  (PWM_BASE+0X064)        /* PWM3��Ƶ�Ĵ��� */
#define PWM3_PERIOD               (PWM_BASE+0X068)        /* PWM3���ڼĴ��� */
#define PWM3_DATA                 (PWM_BASE+0X06C)        /* PWM3���ݼĴ��� */
#define PWM3_CNT                  (PWM_BASE+0X070)        /* PWM3�����Ĵ��� */
#define PWM3_STATUS               (PWM_BASE+0X074)        /* PWM3״̬�Ĵ��� */
#define PWM_INTMASK               (PWM_BASE+0X080)        /* PWM�ж����μĴ��� */
#define PWM_INT                   (PWM_BASE+0X084)        /* PWM�жϼĴ��� */
#define PWM_ENABLE                (PWM_BASE+0X088)        /* PWMʹ�ܼĴ��� */


/*
 *  UART0ģ��
 *  ��ַ: 0x10005000
 */

#define UART0_DLBL                (UART0_BASE+0X000)      /* ���������õͰ�λ�Ĵ��� */
#define UART0_RXFIFO              (UART0_BASE+0X000)      /* ����FIFO */
#define UART0_TXFIFO              (UART0_BASE+0X000)      /* ����FIFO */
#define UART0_DLBH                (UART0_BASE+0X004)      /* ���������ø߰�λ�Ĵ��� */
#define UART0_IER                 (UART0_BASE+0X004)      /* �ж�ʹ�ܼĴ��� */
#define UART0_IIR                 (UART0_BASE+0X008)      /* �ж�ʶ��Ĵ��� */
#define UART0_FCR                 (UART0_BASE+0X008)      /* FIFO���ƼĴ��� */
#define UART0_LCR                 (UART0_BASE+0X00C)      /* �п��ƼĴ��� */
#define UART0_MCR                 (UART0_BASE+0X010)      /* Modem���ƼĴ��� */
#define UART0_LSR                 (UART0_BASE+0X014)      /* ��״̬�Ĵ��� */
#define UART0_MSR                 (UART0_BASE+0X018)      /* Modem״̬�Ĵ��� */


/*
 *  UART1ģ��
 *  ��ַ: 0x10006000
 */

#define UART1_DLBL                (UART1_BASE+0X000)      /* ���������õͰ�λ�Ĵ��� */
#define UART1_RXFIFO              (UART1_BASE+0X000)      /* ����FIFO */
#define UART1_TXFIFO              (UART1_BASE+0X000)      /* ����FIFO */
#define UART1_DLBH                (UART1_BASE+0X004)      /* ���������ø߰�λ�Ĵ��� */
#define UART1_IER                 (UART1_BASE+0X004)      /* �ж�ʹ�ܼĴ��� */
#define UART1_IIR                 (UART1_BASE+0X008)      /* �ж�ʶ��Ĵ��� */
#define UART1_FCR                 (UART1_BASE+0X008)      /* FIFO���ƼĴ��� */
#define UART1_LCR                 (UART1_BASE+0X00C)      /* �п��ƼĴ��� */
#define UART1_MCR                 (UART1_BASE+0X010)      /* Modem���ƼĴ��� */
#define UART1_LSR                 (UART1_BASE+0X014)      /* ��״̬�Ĵ��� */
#define UART1_MSR                 (UART1_BASE+0X018)      /* Modem״̬�Ĵ��� */


/*
 *  UART2ģ��
 *  ��ַ: 0x10007000
 */

#define UART2_DLBL                (UART2_BASE+0X000)      /* ���������õͰ�λ�Ĵ��� */
#define UART2_RXFIFO              (UART2_BASE+0X000)      /* ����FIFO */
#define UART2_TXFIFO              (UART2_BASE+0X000)      /* ����FIFO */
#define UART2_DLBH                (UART2_BASE+0X004)      /* ���������ø߰�λ�Ĵ��� */
#define UART2_IER                 (UART2_BASE+0X004)      /* �ж�ʹ�ܼĴ��� */
#define UART2_IIR                 (UART2_BASE+0X008)      /* �ж�ʶ��Ĵ��� */
#define UART2_FCR                 (UART2_BASE+0X008)      /* FIFO���ƼĴ��� */
#define UART2_LCR                 (UART2_BASE+0X00C)      /* �п��ƼĴ��� */
#define UART2_MCR                 (UART2_BASE+0X010)      /* Modem���ƼĴ��� */
#define UART2_LSR                 (UART2_BASE+0X014)      /* ��״̬�Ĵ��� */
#define UART2_MSR                 (UART2_BASE+0X018)      /* Modem״̬�Ĵ��� */


/*
 *  UART3ģ��
 *  ��ַ: 0x10008000
 */

#define UART3_DLBL                (UART3_BASE+0X000)      /* ���������õͰ�λ�Ĵ��� */
#define UART3_RXFIFO              (UART3_BASE+0X000)      /* ����FIFO */
#define UART3_TXFIFO              (UART3_BASE+0X000)      /* ����FIFO */
#define UART3_DLBH                (UART3_BASE+0X004)      /* ���������ø߰�λ�Ĵ��� */
#define UART3_IER                 (UART3_BASE+0X004)      /* �ж�ʹ�ܼĴ��� */
#define UART3_IIR                 (UART3_BASE+0X008)      /* �ж�ʶ��Ĵ��� */
#define UART3_FCR                 (UART3_BASE+0X008)      /* FIFO���ƼĴ��� */
#define UART3_LCR                 (UART3_BASE+0X00C)      /* �п��ƼĴ��� */
#define UART3_MCR                 (UART3_BASE+0X010)      /* Modem���ƼĴ��� */
#define UART3_LSR                 (UART3_BASE+0X014)      /* ��״̬�Ĵ��� */
#define UART3_MSR                 (UART3_BASE+0X018)      /* Modem״̬�Ĵ��� */


/*
 *  SSIģ��
 *  ��ַ: 0x10009000
 */

#define SSI_CONTROL0              (SSI_BASE+0X000)        /* ���ƼĴ���0 */
#define SSI_CONTROL1              (SSI_BASE+0X004)        /* ���ƼĴ���1 */
#define SSI_SSIENR                (SSI_BASE+0X008)        /* SSIʹ�ܼĴ��� */
#define SSI_MWCR                  (SSI_BASE+0X00C)        /* Microwire���ƼĴ��� */
#define SSI_SER                   (SSI_BASE+0X010)        /* ���豸ʹ�ܼĴ��� */
#define SSI_BAUDR                 (SSI_BASE+0X014)        /* ���������üĴ��� */
#define SSI_TXFTLR                (SSI_BASE+0X018)        /* ����FIFO��ֵ�Ĵ��� */
#define SSI_RXFTLR                (SSI_BASE+0X01C)        /* ����FIFO��ֵ�Ĵ��� */
#define SSI_TXFLR                 (SSI_BASE+0X020)        /* ����FIFO״̬�Ĵ��� */
#define SSI_RXFLR                 (SSI_BASE+0X024)        /* ����FIFO״̬�Ĵ��� */
#define SSI_SR                    (SSI_BASE+0X028)        /* ״̬�Ĵ��� */
#define SSI_IMR                   (SSI_BASE+0X02C)        /* �ж����μĴ��� */
#define SSI_ISR                   (SSI_BASE+0X030)        /* �ж�����״̬�Ĵ��� */
#define SSI_RISR                  (SSI_BASE+0X034)        /* �ж�ԭʼ״̬�Ĵ��� */
#define SSI_TXOICR                (SSI_BASE+0X038)        /* ����FIFO�����ж�����Ĵ��� */
#define SSI_RXOICR                (SSI_BASE+0X03C)        /* ����FIFO�����ж�����Ĵ��� */
#define SSI_RXUICR                (SSI_BASE+0X040)        /* ����FIFO�����ж�����Ĵ��� */
#define SSI_ICR                   (SSI_BASE+0X02C)        /* �ж�����Ĵ��� */
#define SSI_DMACR                 (SSI_BASE+0X04C)        /* DMA���ƼĴ��� */
#define SSI_DMATDLR               (SSI_BASE+0X050)        /* DMA����״̬�Ĵ��� */
#define SSI_DMARDLR               (SSI_BASE+0X054)        /* DMA����״̬�Ĵ��� */
#define SSI_DR                    (SSI_BASE+0X060)        /* ���ݼĴ��� */


/*
 *  I2Sģ��
 *  ��ַ: 0x1000A000
 */

#define I2S_CTRL                  (I2S_BASE+0X000)        /* I2S���ƼĴ��� */
#define I2S_DATA                  (I2S_BASE+0X004)        /* I2S���ݼĴ��� */
#define I2S_INT                   (I2S_BASE+0X008)        /* I2S�жϼĴ��� */
#define I2S_STATUS                (I2S_BASE+0X00C)        /* I2S״̬�Ĵ��� */


/*
 *  SDģ��
 *  ��ַ: 0x1000B000
 */
 
#define SDC_CLOCK_CONTROL  		  (SD_BASE+0x00)          /* SDIOʱ�ӿ��ƼĴ��� */
#define SDC_SOFTWARE_RESET 		  (SD_BASE+0X04)          /* SDIO�����λ�Ĵ��� */
#define SDC_ARGUMENT     		  (SD_BASE+0X08)          /* SDIO��������Ĵ��� */
#define SDC_COMMAND       	      (SD_BASE+0X0C)          /* SDIO������ƼĴ��� */
#define SDC_BLOCK_SIZE            (SD_BASE+0X10)          /* SDIO���ݿ鳤�ȼĴ��� */
#define SDC_BLOCK_COUNT    		  (SD_BASE+0X14)          /* SDIO���ݿ���Ŀ�Ĵ��� */
#define SDC_TRANSFER_MODE  		  (SD_BASE+0X18)          /* SDIO����ģʽѡ��Ĵ��� */
#define SDC_RESPONSE0    		  (SD_BASE+0X1c)          /* SDIO��Ӧ�Ĵ���0 */
#define SDC_RESPONSE1    		  (SD_BASE+0X20)          /* SDIO��Ӧ�Ĵ���1 */
#define SDC_RESPONSE2    		  (SD_BASE+0X24)          /* SDIO��Ӧ�Ĵ���2 */
#define SDC_RESPONSE3    		  (SD_BASE+0X28)          /* SDIO��Ӧ�Ĵ���3 */
#define SDC_READ_TIMEOUT_CONTROL  (SD_BASE+0X2c)          /* SDIO����ʱ���ƼĴ��� */
#define SDC_INTERRUPT_STATUS      (SD_BASE+0X30)          /* SDIO�ж�״̬�Ĵ��� */
#define SDC_INTERRUPT_STATUS_MASK (SD_BASE+0X34)          /* SDIO�ж�״̬���μĴ��� */
#define SDC_READ_BUFER_ACCESS     (SD_BASE+0X38)          /* SDIO����FIFO */
#define SDC_WRITE_BUFER_ACCESS    (SD_BASE+0X3c)          /* SDIO����FIFO */



/*
 *  SMC0ģ��
 *  ��ַ: 0x1000C000
 */

#define SMC0_CTRL                 (SMC0_BASE+0X000)       /* SMC0���ƼĴ��� */
#define SMC0_INT                  (SMC0_BASE+0X004)       /* SMC0�жϼĴ��� */
#define SMC0_FD                   (SMC0_BASE+0X008)       /* SMC0������Ԫʱ��Ĵ��� */
#define SMC0_CT                   (SMC0_BASE+0X00C)       /* SMC0�ַ�����ʱ��Ĵ��� */
#define SMC0_BT                   (SMC0_BASE+0X010)       /* SMC0�鴫��ʱ��Ĵ��� */



/*
 *  SMC1ģ��
 *  ��ַ: 0x1000D000
 */

#define SMC1_CTRL                 (SMC1_BASE+0X000)       /* SMC1���ƼĴ��� */
#define SMC1_INT                  (SMC1_BASE+0X004)       /* SMC1�жϼĴ��� */
#define SMC1_FD                   (SMC1_BASE+0X008)       /* SMC1������Ԫʱ��Ĵ��� */
#define SMC1_CT                   (SMC1_BASE+0X00C)       /* SMC1�ַ�����ʱ��Ĵ��� */
#define SMC1_BT                   (SMC1_BASE+0X010)       /* SMC1�鴫��ʱ��Ĵ��� */



/*
 *  USBDģ��
 *  ��ַ: 0x1000E000
 */

#define USBD_PROTOCOLINTR         (USBD_BASE+0X000)       /* USBЭ���жϼĴ��� */
#define USBD_INTRMASK             (USBD_BASE+0X004)       /* USB�ж����μĴ��� */
#define USBD_INTRCTRL             (USBD_BASE+0X008)       /* USB�ж����Ϳ��ƼĴ��� */
#define USBD_EPINFO               (USBD_BASE+0X00C)       /* USB��˵�״̬�Ĵ��� */
#define USBD_BCONFIGURATIONVALUE  (USBD_BASE+0X010)       /* SET_CCONFIGURATION��¼ */
#define USBD_BMATTRIBUTES         (USBD_BASE+0X014)       /* ��ǰ�������ԼĴ��� */
#define USBD_DEVSPEED             (USBD_BASE+0X018)       /* ��ǰ�豸�����ٶȼĴ��� */
#define USBD_FRAMENUMBER          (USBD_BASE+0X01C)       /* ��¼��ǰSOF���ڵ�֡�� */
#define USBD_EPTRANSACTIONS0      (USBD_BASE+0X020)       /* ��¼�´�Ҫ��Ĵ������ */
#define USBD_EPTRANSACTIONS1      (USBD_BASE+0X024)       /* ��¼�´�Ҫ��Ĵ������ */
#define USBD_APPIFUPDATE          (USBD_BASE+0X028)       /* �ӿںſ��ٸ��¼Ĵ��� */
#define USBD_CFGINTERFACE0        (USBD_BASE+0X02C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE1        (USBD_BASE+0X030)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE2        (USBD_BASE+0X034)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE3        (USBD_BASE+0X038)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE4        (USBD_BASE+0X03C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE5        (USBD_BASE+0X040)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE6        (USBD_BASE+0X044)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE7        (USBD_BASE+0X048)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE8        (USBD_BASE+0X04C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE9        (USBD_BASE+0X050)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE10       (USBD_BASE+0X054)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE11       (USBD_BASE+0X058)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE12       (USBD_BASE+0X05C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE13       (USBD_BASE+0X060)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE14       (USBD_BASE+0X064)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE15       (USBD_BASE+0X068)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE16       (USBD_BASE+0X06C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE17       (USBD_BASE+0X070)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE18       (USBD_BASE+0X074)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE19       (USBD_BASE+0X078)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE20       (USBD_BASE+0X07C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE21       (USBD_BASE+0X080)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE22       (USBD_BASE+0X084)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE23       (USBD_BASE+0X088)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE24       (USBD_BASE+0X08C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE25       (USBD_BASE+0X090)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE26       (USBD_BASE+0X094)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE27       (USBD_BASE+0X098)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE28       (USBD_BASE+0X09C)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE29       (USBD_BASE+0X0A0)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE30       (USBD_BASE+0X0A4)       /* ��¼�ӿڵ�ֵ */
#define USBD_CFGINTERFACE31       (USBD_BASE+0X0A8)       /* ��¼�ӿڵ�ֵ */
#define USBD_PKTPASSEDCTRL        (USBD_BASE+0X0AC)       /* ��¼�ɹ����յİ��� */
#define USBD_PKTDROPPEDCTRL       (USBD_BASE+0X0B0)       /* ��¼��ʧ�İ��� */
#define USBD_CRCERRCTRL           (USBD_BASE+0X0B4)       /* ��¼CRC����İ��� */
#define USBD_BITSTUFFERRCTRL      (USBD_BASE+0X0B8)       /* ��¼λ������İ��� */
#define USBD_PIDERRCTRL           (USBD_BASE+0X0BC)       /* ��¼PID����İ��� */
#define USBD_FRAMINGERRCTL        (USBD_BASE+0X0C0)       /* ��¼��SYNC��EOP�İ��� */
#define USBD_TXPKTCTRL            (USBD_BASE+0X0C4)       /* ��¼���Ͱ������� */
#define USBD_STATCTRLOV           (USBD_BASE+0X0C8)       /* ��¼ͳ�ƼĴ��������� */
#define USBD_TXLENGTH             (USBD_BASE+0X0CC)       /* ��¼ÿ��IN������������� */
#define USBD_RXLENGTH             (USBD_BASE+0X0D0)       /* ��¼OUT������������� */
#define USBD_RESUME               (USBD_BASE+0X0D4)       /* USB���ѼĴ��� */
#define USBD_READFLAG             (USBD_BASE+0X0D8)       /* ���첽״̬�Ĵ�����־ */
#define USBD_RECEIVETYPE          (USBD_BASE+0X0DC)       /* ����״̬�Ĵ��� */
#define USBD_APPLOCK              (USBD_BASE+0X0E0)       /* ���źżĴ��� */
#define USBD_EP0OUTADDR           (USBD_BASE+0X100)       /* �˵�0�˵�źͷ��� */
#define USBD_EP0OUTBMATTR         (USBD_BASE+0X104)       /* �˵�0���ͼĴ��� */
#define USBD_EP0OUTMAXPKTSIZE     (USBD_BASE+0X108)       /* �˵�0�����ߴ�Ĵ��� */
#define USBD_EP0OUTIFNUM          (USBD_BASE+0X10C)       /* �˵�0�ӿںżĴ��� */
#define USBD_EP0OUTSTAT           (USBD_BASE+0X110)       /* �˵�0״̬�Ĵ��� */
#define USBD_EP0OUTBMREQTYPE      (USBD_BASE+0X114)       /* �˵�0 SETUP���������� */
#define USBD_EP0OUTBREQUEST       (USBD_BASE+0X118)       /* �˵�0 SETUP������������ */
#define USBD_EP0OUTWVALUE         (USBD_BASE+0X11C)       /* �˵�0 SETUP��������ֵ */
#define USBD_EP0OUTWINDEX         (USBD_BASE+0X120)       /* �˵�0 SETUP������������ */
#define USBD_EP0OUTWLENGTH        (USBD_BASE+0X120)       /* �˵�0 SETUP�������󳤶� */
#define USBD_EP0OUTSYNCHFRAME     (USBD_BASE+0X128)       /* �˵�0ͬ����֡�� */
#define USBD_EP1OUTADDR           (USBD_BASE+0X12C)       /* �˵�1����˵�źͷ��� */
#define USBD_EP1OUTBMATTR         (USBD_BASE+0X130)       /* �˵�1������ͼĴ��� */
#define USBD_EP1OUTMAXPKTSIZE     (USBD_BASE+0X134)       /* �˵�1��������ߴ�Ĵ��� */
#define USBD_EP1OUTIFNUM          (USBD_BASE+0X138)       /* �˵�1����ӿںżĴ��� */
#define USBD_EP1OUTSTAT           (USBD_BASE+0X13C)       /* �˵�1���״̬�Ĵ��� */
#define USBD_EP1OUTBMREQTYPE      (USBD_BASE+0X140)       /* �˵�1���SETUP������������ */
#define USBD_EP1OUTBREQUEST       (USBD_BASE+0X144)       /* �˵�1���SETUP������������ */
#define USBD_EP1OUTWVALUE         (USBD_BASE+0X148)       /* �˵�1���SETUP��������ֵ */
#define USBD_EP1OUTWINDX          (USBD_BASE+0X14C)       /* �˵�1���SETUP������������ */
#define USBD_EP1OUTWLENGH         (USBD_BASE+0X150)       /* �˵�1���SETUP���������򳤶� */
#define USBD_EP1OUTSYNCHFRAME     (USBD_BASE+0X154)       /* �˵�1���ͬ����֡�� */
#define USBD_EP1INADDR            (USBD_BASE+0X158)       /* �˵�1����˵�źͷ��� */
#define USBD_EP1INBMATTR          (USBD_BASE+0X15C)       /* �˵�1�������ͼĴ��� */
#define USBD_EP1INMAXPKTSIZE      (USBD_BASE+0X160)       /* �˵�1���������ߴ�Ĵ��� */
#define USBD_EP1INIFNUM           (USBD_BASE+0X164)       /* �˵�1����ӿںżĴ��� */
#define USBD_EP1INSTAT            (USBD_BASE+0X168)       /* �˵�1����״̬�Ĵ��� */
#define USBD_EP1INBMREQTYPE       (USBD_BASE+0X16C)       /* �˵�1����SETUP������������ */
#define USBD_EP1INBREQUEST        (USBD_BASE+0X170)       /* �˵�1����SETUP������������ */
#define USBD_EP1INWVALUE          (USBD_BASE+0X174)       /* �˵�1����SETUP��������ֵ */
#define USBD_EP1INWINDEX          (USBD_BASE+0X178)       /* �˵�1����SETUP������������ */
#define USBD_EP1INWLENGTH         (USBD_BASE+0X17C)       /* �˵�1����SETUP���������򳤶� */
#define USBD_EP1INSYNCHFRAME      (USBD_BASE+0X180)       /* �˵�1����ͬ����֡�� */
#define USBD_EP2OUTADDR           (USBD_BASE+0X184)       /* �˵�2����˵�źͷ��� */
#define USBD_EP2OUTBMATTR         (USBD_BASE+0X188)       /* �˵�2������ͼĴ��� */
#define USBD_EP2OUTMAXPKTSIZE     (USBD_BASE+0X18C)       /* �˵�2��������ߴ�Ĵ��� */
#define USBD_EP2OUTIFNUM          (USBD_BASE+0X190)       /* �˵�2����ӿںżĴ��� */
#define USBD_EP2OUTSTAT           (USBD_BASE+0X194)       /* �˵�2���״̬�Ĵ��� */
#define USBD_EP2OUTBMREQTYPE      (USBD_BASE+0X198)       /* �˵�2���SETUP������������ */
#define USBD_EP2OUTBREQUEST       (USBD_BASE+0X19C)       /* �˵�2���SETUP������������ */
#define USBD_EP2OUTWVALUE         (USBD_BASE+0X1A0)       /* �˵�2���SETUP��������ֵ */
#define USBD_EP2OUTWINDEX         (USBD_BASE+0X1A4)       /* �˵�2���SETUP������������ */
#define USBD_EP2OUTWLENGTH        (USBD_BASE+0X1A8)       /* �˵�2���SETUP���������򳤶� */
#define USBD_EP2OUTSYNCHFRAME     (USBD_BASE+0X1AC)       /* �˵�2���ͬ����֡�� */
#define USBD_EP2INADDR            (USBD_BASE+0X1B0)       /* �˵�2����˵�źͷ��� */
#define USBD_EP2INBMATTR          (USBD_BASE+0X1B4)       /* �˵�2�������ͼĴ��� */
#define USBD_EP2INMAXPKTSIZE      (USBD_BASE+0X1B8)       /* �˵�2���������ߴ�Ĵ��� */
#define USBD_EP2INIFNUM           (USBD_BASE+0X1BC)       /* �˵�2����ӿںżĴ��� */
#define USBD_EP2INSTAT            (USBD_BASE+0X1C0)       /* �˵�2����״̬�Ĵ��� */
#define USBD_EP2INBMREQTYPE       (USBD_BASE+0X1C4)       /* �˵�2����SETUP������������ */
#define USBD_EP2INBREQUEST        (USBD_BASE+0X1C8)       /* �˵�2����SETUP������������ */
#define USBD_EP2INWVALUE          (USBD_BASE+0X1CC)       /* �˵�2����SETUP��������ֵ */
#define USBD_EP2INWINDEX          (USBD_BASE+0X1D0)       /* �˵�2����SETUP������������ */
#define USBD_EP2INWLENGTH         (USBD_BASE+0X1D4)       /* �˵�2����SETUP���������򳤶� */
#define USBD_EP2INSYNCHFRAME      (USBD_BASE+0X1D8)       /* �˵�2����ͬ����֡�� */
#define USBD_RXFIFO               (USBD_BASE+0X200)       /* ����FIFO */
#define USBD_TXFIFO               (USBD_BASE+0X300)       /* ����FIFO */


/*
 *  GPIOģ��
 *  ��ַ: 0x1000F000
 */

#define GPIO_DBCLK_DIV            (GPIO_BASE+0X000)       /* ȥë�̲���ʱ�ӷ�Ƶ�����üĴ��� */
#define GPIO_PORTA_DIR            (GPIO_BASE+0X004)       /* A��˿���������������üĴ��� */
#define GPIO_PORTA_SEL            (GPIO_BASE+0X008)       /* A��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTA_INCTL          (GPIO_BASE+0X00C)       /* A��˿�ͨ����;����ʱ�������üĴ��� */
#define GPIO_PORTA_INTRCTL        (GPIO_BASE+0X010)       /* A��˿��жϴ����������üĴ��� */
#define GPIO_PORTA_INTRCLR        (GPIO_BASE+0X014)       /* A��˿�ͨ����;�ж�������üĴ��� */
#define GPIO_PORTA_DATA           (GPIO_BASE+0X018)       /* A��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTB_DIR            (GPIO_BASE+0X01C)       /* B��˿���������������üĴ��� */
#define GPIO_PORTB_SEL            (GPIO_BASE+0X020)       /* B��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTB_DATA           (GPIO_BASE+0X024)       /* B��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTC_DIR            (GPIO_BASE+0X028)       /* C��˿���������������üĴ��� */
#define GPIO_PORTC_SEL            (GPIO_BASE+0X02C)       /* C��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTC_DATA           (GPIO_BASE+0X030)       /* C��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTD_DIR            (GPIO_BASE+0X034)       /* D��˿���������������üĴ��� */
#define GPIO_PORTD_SEL            (GPIO_BASE+0X038)       /* D��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTD_SPECII         (GPIO_BASE+0X03C)       /* D��˿�ר����;2ѡ�����üĴ��� */
#define GPIO_PORTD_DATA           (GPIO_BASE+0X040)       /* D��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTE_DIR            (GPIO_BASE+0X044)       /* E��˿���������������üĴ��� */
#define GPIO_PORTE_SEL            (GPIO_BASE+0X048)       /* E��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTE_DATA           (GPIO_BASE+0X04C)       /* E��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTF_DIR            (GPIO_BASE+0X050)       /* F��˿���������������üĴ��� */
#define GPIO_PORTF_SEL            (GPIO_BASE+0X054)       /* F��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTF_INCTL          (GPIO_BASE+0X058)       /* F��˿�ͨ����;����ʱ�������üĴ��� */
#define GPIO_PORTF_INTRCTL        (GPIO_BASE+0X05C)       /* F��˿��жϴ����������üĴ��� */
#define GPIO_PORTF_INTRCLR        (GPIO_BASE+0X060)       /* F��˿�ͨ����;�ж�������üĴ��� */
#define GPIO_PORTF_DATA           (GPIO_BASE+0X064)       /* F��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTG_DIR            (GPIO_BASE+0X068)       /* G��˿���������������üĴ��� */
#define GPIO_PORTG_SEL            (GPIO_BASE+0X06C)       /* G��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTG_DATA           (GPIO_BASE+0X070)       /* G��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTH_DIR            (GPIO_BASE+0X07C)       /* H��˿���������������üĴ��� */
#define GPIO_PORTH_SEL            (GPIO_BASE+0X078)       /* H��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTH_DATA           (GPIO_BASE+0X07C)       /* H��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTI_DIR            (GPIO_BASE+0X080)       /* I��˿���������������üĴ��� */
#define GPIO_PORTI_SEL            (GPIO_BASE+0X084)       /* I��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTI_DATA           (GPIO_BASE+0X088)       /* I��˿�ͨ����;�������üĴ��� */



/*
 *  EMIģ��
 *  ��ַ: 0x11000000
 */

#define EMI_CSACONF               (EMI_BASE+0X000)        /* CSA�������üĴ��� */
#define EMI_CSBCONF               (EMI_BASE+0X004)        /* CSB�������üĴ��� */
#define EMI_CSCCONF               (EMI_BASE+0X008)        /* CSC�������üĴ��� */
#define EMI_CSDCONF               (EMI_BASE+0X00C)        /* CSD�������üĴ��� */
#define EMI_CSECONF               (EMI_BASE+0X010)        /* CSE�������üĴ��� */
#define EMI_CSFCONF               (EMI_BASE+0X014)        /* CSF�������üĴ��� */
#define EMI_SDCONF1               (EMI_BASE+0X018)        /* SDRAMʱ�����üĴ���1 */
#define EMI_SDCONF2               (EMI_BASE+0X01C)        /* SDRAMʱ�����üĴ���2, SDRAM��ʼ���õ���������Ϣ */
#define EMI_REMAPCONF             (EMI_BASE+0X020)        /* Ƭѡ�ռ估��ַӳ��REMAP���üĴ��� */
#define EMI_NAND_ADDR1            (EMI_BASE+0X100)        /* NAND FLASH�ĵ�ַ�Ĵ���1 */
#define EMI_NAND_COM              (EMI_BASE+0X104)        /* NAND FLASH�Ŀ����ּĴ��� */
#define EMI_NAND_STA              (EMI_BASE+0X10C)        /* NAND FLASH��״̬�Ĵ��� */
#define EMI_ERR_ADDR1             (EMI_BASE+0X110)        /* ����������ĵ�ַ�Ĵ���1 */
#define EMI_ERR_ADDR2             (EMI_BASE+0X114)        /* ����������ĵ�ַ�Ĵ���2 */
#define EMI_NAND_CONF1            (EMI_BASE+0X118)        /* NAND FLASH������������1 */
#define EMI_NAND_INTR             (EMI_BASE+0X11C)        /* NAND FLASH�жϼĴ��� */
#define EMI_NAND_ECC              (EMI_BASE+0X120)        /* ECCУ����ɼĴ��� */
#define EMI_NAND_IDLE             (EMI_BASE+0X124)        /* NAND FLASH���мĴ��� */
#define EMI_NAND_CONF2            (EMI_BASE+0X128)        /* NAND FLASH������������2 */
#define EMI_NAND_ADDR2            (EMI_BASE+0X12C)        /* NAND FLASH�ĵ�ַ�Ĵ���2 */
#define EMI_NAND_DATA             (EMI_BASE+0X200)        /* NAND FLASH�����ݼĴ��� */


/*
 *  DMACģ��
 *  ��ַ: 0x11001000
 */

#define DMAC_INTSTATUS            (DMAC_BASE+0X020)       /* DAMC�ж�״̬�Ĵ����� */
#define DMAC_INTTCSTATUS          (DMAC_BASE+0X050)       /* DMAC��������ж�״̬�Ĵ��� */
#define DMAC_INTTCCLEAR           (DMAC_BASE+0X060)       /* DMAC��������ж�״̬����Ĵ��� */
#define DMAC_INTERRORSTATUS       (DMAC_BASE+0X080)       /* DMAC��������ж�״̬�Ĵ��� */
#define DMAC_INTINTERRCLR         (DMAC_BASE+0X090)       /* DMAC��������ж�״̬����Ĵ��� */
#define DMAC_ENBLDCHNS            (DMAC_BASE+0X0B0)       /* DMACͨ��ʹ��״̬�Ĵ��� */
#define DMAC_C0SRCADDR            (DMAC_BASE+0X000)       /* DMAC��0Դ��ַ�Ĵ��� */
#define DMAC_C0DESTADD            (DMAC_BASE+0X004)       /* DMAC��0Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C0CONTROL            (DMAC_BASE+0X00C)       /* DMAC��0���ƼĴ��� */
#define DMAC_C0CONFIGURATION      (DMAC_BASE+0X010)       /* DMAC��0���üĴ��� */
#define DMAC_C0DESCRIPTOR         (DMAC_BASE+0X01C)       /* DMAC��0�����ַ�Ĵ��� */
#define DMAC_C1SRCADDR            (DMAC_BASE+0X100)       /* DMAC��1Դ��ַ�Ĵ��� */
#define DMAC_C1DESTADDR           (DMAC_BASE+0X104)       /* DMAC��1Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C1CONTROL            (DMAC_BASE+0X10C)       /* DMAC��1���ƼĴ��� */
#define DMAC_C1CONFIGURATION      (DMAC_BASE+0X110)       /* DMAC��1���üĴ��� */
#define DMAC_C1DESCRIPTOR         (DMAC_BASE+0X114)       /* DMAC��1�����ַ�Ĵ��� */
#define DMAC_C2SRCADDR            (DMAC_BASE+0X200)       /* DMAC��2Դ��ַ�Ĵ��� */
#define DMAC_C2DESTADDR           (DMAC_BASE+0X204)       /* DMAC��2Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C2CONTROL            (DMAC_BASE+0X20C)       /* DMAC��2���ƼĴ��� */
#define DMAC_C2CONFIGURATION      (DMAC_BASE+0X210)       /* DMAC��2���üĴ��� */
#define DMAC_C2DESCRIPTOR         (DMAC_BASE+0X214)       /* DMAC��2�����ַ�Ĵ��� */
#define DMAC_C3SRCADDR            (DMAC_BASE+0X300)       /* DMAC��3Դ��ַ�Ĵ��� */
#define DMAC_C3DESTADDR           (DMAC_BASE+0X304)       /* DMAC��3Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C3CONTROL            (DMAC_BASE+0X30C)       /* DMAC��3���ƼĴ��� */
#define DMAC_C3CONFIGURATION      (DMAC_BASE+0X310)       /* DMAC��3���üĴ��� */
#define DMAC_C3DESCRIPTOR         (DMAC_BASE+0X314)       /* DMAC��3�����ַ�Ĵ��� */
#define DMAC_C4SRCADDR            (DMAC_BASE+0X400)       /* DMAC��4Դ��ַ�Ĵ��� */
#define DMAC_C4DESTADDR           (DMAC_BASE+0X404)       /* DMAC��4Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C4CONTROL            (DMAC_BASE+0X40C)       /* DMAC��4���ƼĴ��� */
#define DMAC_C4CONFIGURATION      (DMAC_BASE+0X410)       /* DMAC��4���üĴ��� */
#define DMAC_C4DESCRIPTOR         (DMAC_BASE+0X414)       /* DMAC��4�����ַ�Ĵ��� */
#define DMAC_C5SRCADDR            (DMAC_BASE+0X500)       /* DMAC��5Դ��ַ�Ĵ��� */
#define DMAC_C5DESTADDR           (DMAC_BASE+0X504)       /* DMAC��5Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C5CONTROL            (DMAC_BASE+0X50C)       /* DMAC��5���ƼĴ��� */
#define DMAC_C5CONFIGURATION      (DMAC_BASE+0X510)       /* DMAC��5���üĴ��� */
#define DMAC_C5DESCRIPTOR         (DMAC_BASE+0X514)       /* DMAC��5�����ַ�Ĵ��� */


/*
 *  LCDCģ��
 *  ��ַ: 0x11002000
 */

#define LCDC_SSA                  (LCDC_BASE+0X000)       /* ��Ļ��ʼ��ַ�Ĵ��� */
#define LCDC_SIZE                 (LCDC_BASE+0X004)       /* ��Ļ�ߴ�Ĵ��� */
#define LCDC_PCR                  (LCDC_BASE+0X008)       /* ������üĴ��� */
#define LCDC_HCR                  (LCDC_BASE+0X00C)       /* ˮƽ���üĴ��� */
#define LCDC_VCR                  (LCDC_BASE+0X010)       /* ��ֱ���üĴ��� */
#define LCDC_PWMR                 (LCDC_BASE+0X014)       /* PWM�Աȶȿ��ƼĴ��� */
#define LCDC_LECR                 (LCDC_BASE+0X018)       /* ʹ�ܿ��ƼĴ��� */
#define LCDC_DMACR                (LCDC_BASE+0X01C)       /* DMA���ƼĴ��� */
#define LCDC_LCDISREN             (LCDC_BASE+0X020)       /* �ж�ʹ�ܼĴ��� */
#define LCDC_LCDISR               (LCDC_BASE+0X024)       /* �ж�״̬�Ĵ��� */
#define LCDC_LGPMR                (LCDC_BASE+0X040)       /* �Ҷȵ�ɫӳ��Ĵ����� (16��32bit�Ĵ���) */


/*
 *  MACģ��
 *  ��ַ: 0x11003000
 */

#define MAC_CTRL                  (MAC_BASE+0X000)        /* MAC���ƼĴ��� */
#define MAC_INTSRC                (MAC_BASE+0X004)        /* MAC�ж�Դ�Ĵ��� */
#define MAC_INTMASK               (MAC_BASE+0X008)        /* MAC�ж����μĴ��� */
#define MAC_IPGT                  (MAC_BASE+0X00C)        /* ����֡����Ĵ��� */
#define MAC_IPGR1                 (MAC_BASE+0X010)        /* �ȴ����ڼĴ��� */
#define MAC_IPGR2                 (MAC_BASE+0X014)        /* �ȴ����ڼĴ��� */
#define MAC_PACKETLEN             (MAC_BASE+0X018)        /* ֡���ȼĴ��� */
#define MAC_COLLCONF              (MAC_BASE+0X01C)        /* ��ײ�ط��Ĵ��� */
#define MAC_TXBD_NUM              (MAC_BASE+0X020)        /* �����������Ĵ��� */
#define MAC_FLOWCTRL              (MAC_BASE+0X024)        /* ���ؼĴ��� */
#define MAC_MII_CTRL              (MAC_BASE+0X028)        /* PHY���ƼĴ��� */
#define MAC_MII_CMD               (MAC_BASE+0X02C)        /* PHY����Ĵ��� */
#define MAC_MII_ADDRESS           (MAC_BASE+0X030)        /* PHY��ַ�Ĵ��� */
#define MAC_MII_TXDATA            (MAC_BASE+0X034)        /* PHYд���ݼĴ��� */
#define MAC_MII_RXDATA            (MAC_BASE+0X038)        /* PHY�����ݼĴ��� */
#define MAC_MII_STATUS            (MAC_BASE+0X03C)        /* PHY״̬�Ĵ��� */
#define MAC_ADDR0                 (MAC_BASE+0X040)        /* MAC��ַ�Ĵ��� */
#define MAC_ADDR1                 (MAC_BASE+0X044)        /* MAC��ַ�Ĵ��� */
#define MAC_HASH0                 (MAC_BASE+0X048)        /* MAC HASH�Ĵ��� */
#define MAC_HASH1                 (MAC_BASE+0X04C)        /* MAC HASH�Ĵ��� */
#define MAC_TXPAUSE               (MAC_BASE+0X050)        /* MAC����֡�Ĵ��� */
#define MAC_TX_BD                 (MAC_BASE+0X400)      
#define MAC_RX_BD                 (MAC_BASE+0X600)      


/*
 **************************************
 * Error Codes:
 *    IF SUCCESS RETURN 0, ELSE RETURN OTHER ERROR CODE,
 *    parameter error return (-33)/E_PAR, 
 *    hardware error reture (-99)/E_HA
 **************************************
 */

#define    E_OK          0        /* Normal completion */
#define    E_SYS         (-5)     /* System error */
#define    E_NOMEM       (-10)    /* Insufficient memory */
#define    E_NOSPT       (-17)    /* Feature not supported */
#define    E_INOSPT      (-18)    /* Feature not supported by ITRON/FILE specification */
#define    E_RSFN        (-20)    /* Reserved function code number */
#define    E_RSATR       (-24)    /* Reserved attribute */
#define    E_PAR         (-33)    /* Parameter error */
#define    E_ID          (-35)    /* Invalid ID number */
#define    E_NOEXS       (-52)    /* Object does not exist */
#define    E_OBJ         (-63)    /* Invalid object state */
#define    E_MACV        (-65)    /* Memory access disabled or memory access violation */
#define    E_OACV        (-66)    /* Object access violation */
#define    E_CTX         (-69)    /* Context error */
#define    E_QOVR        (-73)    /* Queuing or nesting overflow */
#define    E_DLT         (-81)    /* Object being waited for was deleted */
#define    E_TMOUT       (-85)    /* Polling failure or timeout exceeded */
#define    E_RLWAI       (-86)    /* WAIT state was forcibly released */ 

#define    E_HA          (-99)    /* HARD WARE ERROR */


/*
 **************************************
 * PMU ģ��ʱ��
 **************************************
 */

#define    CLK_SGPT      (1 << 16)
#define    CLK_SI2S      (1 << 15) 
#define    CLK_SSMC      (1 << 14)
#define    CLK_SMAC      (1 << 13)
#define    CLK_SUSB      (1 << 12)
#define    CLK_SUART3    (1 << 11)
#define    CLK_SUART2    (1 << 10)
#define    CLK_SUART1    (1 << 9)
#define    CLK_SUART0    (1 << 8)
#define    CLK_SSSI      (1 << 7)
#define    CLK_SAC97     (1 << 6)
#define    CLK_SMMCSD    (1 << 5)
#define    CLK_SEMI      (1 << 4)
#define    CLK_SDMAC     (1 << 3)
#define    CLK_SPWM      (1 << 2)
#define    CLK_SLCDC     (1 << 1)
#define    CLK_SESRAM    (1)


/*Interrupt Sources*/


#define  INTSRC_RTC        31		        
#define  INTSRC_DMAC       30	                
#define  INTSRC_EMI        29
#define  INTSRC_MAC        28
#define  INTSRC_TIMER1     27
#define  INTSRC_TIMER2     26
#define  INTSRC_TIMER3     25
#define  INTSRC_UART0      24
#define  INTSRC_UART1      23
#define  INTSRC_UART2      22
#define  INTSRC_UART3      21
#define  INTSRC_PWM        20
#define  INTSRC_LCDC       19
#define  INTSRC_I2S        18
#define  INTSRC_SSI        17

#define  INTSRC_USB        15
#define  INTSRC_SMC0       14
#define  INTSRC_SMC1       13
#define  INTSRC_SDIO       12  
#define  INTSRC_EXINT10    11              
#define  INTSRC_EXINT9     10              
#define  INTSRC_EXINT8     9               
#define  INTSRC_EXINT7     8               
#define  INTSRC_EXINT6     7               
#define  INTSRC_EXINT5     6               
#define  INTSRC_EXINT4     5               
#define  INTSRC_EXINT3     4               
#define  INTSRC_EXINT2     3               
#define  INTSRC_EXINT1     2               
#define  INTSRC_EXINT0     1
#define  INTSRC_NULL       0


/*Sereral useful macros*/
#define set_plevel(plevel)          *(RP)INTC_IPLR = plevel                      //������ͨ�жϵ����ȼ����ޣ�ֻ�����ȼ����ڴ�ֵ���жϲ���ͨ��
#define set_int_force(intnum)       *(RP)INTC_IFR = (1 << intnum)                //��1�����ǿ�Ƹ�λ��Ӧ���ж�Դ�����ж��ź�
#define enable_irq(intnum)          *(RP)INTC_IER |= (1 << intnum)               //��1�������ж�Դ��IRQ �ж��ź�
#define disable_irq( intnum)        *(RP)INTC_IER &= ~(1<< intnum)               //��0�󣬲������ж�Դ��IRQ �ж��ź�
#define mask_irq(intnum)            *(RP)INTC_IMR |= (1 << intnum)               //��1�����ζ�Ӧ��IRQ �ж��ź�               
#define unmask_irq(intnum)          *(RP)INTC_IMR &= ~(1 << intnum)              //��0��ͨ����Ӧ��IRQ �ж��ź�
#define mask_all_irq()              *(RP)INTC_IMR = 0xFFFFFFFF                   //���ζ�Ӧ��IRQ �ж��ź�
#define unmask_all_irq()            *(RP)INTC_IMR = 0x00000000                   //ͨ����Ӧ��IRQ �ж��ź�
#define enable_all_irq()            *(RP)INTC_IER = 0XFFFFFFFF                   //�����ж�Դ��IRQ �ж��ź�
#define disable_all_irq()           *(RP)INTC_IER = 0X00000000                   //�������ж�Դ��IRQ �ж��ź�
#define InitInt()                  do{mask_all_irq(); enable_all_irq();}while(0)

/*
 **************************************
 * ���г������õ���Typedef
 **************************************
 */
 
typedef    char                 S8;        /* signed 8-bit integer */
typedef    short                S16;       /* signed 16-bit integer */
typedef    long                 S32;       /* signed 32-bit integer */
typedef    unsigned char        U8;        /* unsigned 8-bit integer */
typedef    unsigned short       U16;       /* unsigned 16-bit integer */
typedef    unsigned long        U32;       /* unsigned 32-bit integer */

typedef    volatile U32 *       RP;
typedef    volatile U16 *       RP16;
typedef    volatile U8  *       RP8;

typedef    void                 *VP;       /* pointer to an unpredictable data type */
typedef    void                 (*FP)();   /* program start address */

#ifndef    _BOOL_TYPE_
#define    _BOOL_TYPE_
typedef    int                  BOOL;	     /* Boolean value.  TRUE (1) or FALSE (0). */
#endif  

typedef    int                  ER;         /* Error code.  A signed integer. */

/**
 * IO definitions
 *
 * define access restrictions to peripheral registers
 */

#define     __I     volatile const            /*!< defines 'read only' permissions      */
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */
#define     __iomem volatile


/*Macros for debug*/

#define EOUT(fmt,...) \
	do \
	{ 	\
		rt_kprintf("EOUT:(%s:%i)  ",__FILE__,__LINE__); \
		rt_kprintf(fmt,##__VA_ARGS__); \
	}while(0)

#define RT_DEBUG
#ifdef RT_DEBUG
	#define DBOUT(fmt,...) \
	do \
	{ 	\
		rt_kprintf("DBOUT:(%s:%i)  ",__FILE__,__LINE__); \
		rt_kprintf(fmt,##__VA_ARGS__); \
	}while(0)
#else
	#define DBOUT(fmt,...) \
	do{}while(0)
#endif

#ifdef	RT_DEBUG
	#define ASSERT(arg) \
	if((arg) == 0) \
	{		\
		while(1) \
		{		  \
			rt_kprintf("have a assert failure\n"); \
		}										 \
	}
#else
	#define ASSERT(arg) \
	do		\
	{		 \
	}while(0)
#endif


#define write_reg(reg,value) \
	do \
	{ \
		*(RP)(reg) = value; \
	}while(0)

#define read_reg(reg) (*(RP)reg)


struct rt_hw_register
{
	rt_uint32_t r0;
	rt_uint32_t r1;
	rt_uint32_t r2;
	rt_uint32_t r3;
	rt_uint32_t r4;
	rt_uint32_t r5;
	rt_uint32_t r6;
	rt_uint32_t r7;
	rt_uint32_t r8;
	rt_uint32_t r9;
	rt_uint32_t r10;
	rt_uint32_t fp;
	rt_uint32_t ip;
	rt_uint32_t sp;
	rt_uint32_t lr;
	rt_uint32_t pc;
	rt_uint32_t cpsr;
	rt_uint32_t ORIG_r0;
};


/*@}*/

#endif
