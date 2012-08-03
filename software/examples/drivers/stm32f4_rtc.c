#include <string.h>
#include <time.h>
#include <rtthread.h>
#include <board.h>
#include "stm32f4_rtc.h"

static struct rt_device rtc;
static rt_err_t rt_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    if (dev->rx_indicate != RT_NULL)
    {
        /* Open Interrupt */
    }

    return RT_EOK;
}

static rt_size_t rt_rtc_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
    return 0;
}

static rt_err_t rt_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    time_t *time;
    RTC_TimeTypeDef   RTC_TimeStructure;
    RTC_DateTypeDef   RTC_DateStructure;
    struct tm time_temp;

    RT_ASSERT(dev != RT_NULL);
    memset(&time_temp, 0, sizeof(struct tm));

    switch (cmd)
    {
    case RT_DEVICE_CTRL_RTC_GET_TIME:
        time = (time_t *)args;

        /* Get the current Time */
        RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
        RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);
        /* Years since 1900 : 0-99 range */
        time_temp.tm_year = RTC_DateStructure.RTC_Year + 2000 - 1900;
        /* Months *since* january 0-11 : RTC_Month_Date_Definitions 1 - 12 */
        time_temp.tm_mon = RTC_DateStructure.RTC_Month - 1;
        /* Day of the month 1-31 : 1-31 range */
        time_temp.tm_mday = RTC_DateStructure.RTC_Date;
        /* Hours since midnight 0-23 : 0-23 range */
        time_temp.tm_hour = RTC_TimeStructure.RTC_Hours;
        /* Minutes 0-59 : the 0-59 range */
        time_temp.tm_min = RTC_TimeStructure.RTC_Minutes;
        /* Seconds 0-59 : the 0-59 range */
        time_temp.tm_sec = RTC_TimeStructure.RTC_Seconds;

        *time = mktime(&time_temp);
        break;

    case RT_DEVICE_CTRL_RTC_SET_TIME:
    {
        const struct tm* time_new;
        time = (time_t *)args;
        time_new = localtime(time);

        /* 0-99 range              : Years since 1900 */
        RTC_DateStructure.RTC_Year = time_new->tm_year + 1900 - 2000;
        /* RTC_Month_Date_Definitions 1 - 12 : Months *since* january 0-11 */
        RTC_DateStructure.RTC_Month = time_new->tm_mon + 1;
        /* 1-31 range : Day of the month 1-31 */
        RTC_DateStructure.RTC_Date = time_new->tm_mday;
        /* 1 - 7 : Days since Sunday (0-6) */
        RTC_DateStructure.RTC_WeekDay = time_new->tm_wday + 1;
        /* 0-23 range : Hours since midnight 0-23 */
        RTC_TimeStructure.RTC_Hours = time_new->tm_hour;
        /* the 0-59 range : Minutes 0-59 */
        RTC_TimeStructure.RTC_Minutes = time_new->tm_min;
        /* the 0-59 range : Seconds 0-59 */
        RTC_TimeStructure.RTC_Seconds = time_new->tm_sec;

        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);

        /* Set Current Time and Date */
        RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
        RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
    }
    break;
    }

    return RT_EOK;
}

/************************************************/
#define RTC_CLOCK_SOURCE_LSE
#define FIRST_DATA          0x32F2

__IO uint32_t AsynchPrediv = 0, SynchPrediv = 0;
__IO uint32_t TimeDisplay = 0;
uint32_t errorindex = 0, i = 0;

#define RTC_BKP_DR_NUMBER   0x14
uint32_t BKPDataReg[RTC_BKP_DR_NUMBER] =
{
    RTC_BKP_DR0, RTC_BKP_DR1, RTC_BKP_DR2,
    RTC_BKP_DR3, RTC_BKP_DR4, RTC_BKP_DR5,
    RTC_BKP_DR6, RTC_BKP_DR7, RTC_BKP_DR8,
    RTC_BKP_DR9, RTC_BKP_DR10, RTC_BKP_DR11,
    RTC_BKP_DR12, RTC_BKP_DR13, RTC_BKP_DR14,
    RTC_BKP_DR15, RTC_BKP_DR16, RTC_BKP_DR17,
    RTC_BKP_DR18,  RTC_BKP_DR19
};
static void WriteToBackupReg(uint16_t FirstBackupData)
{
    uint32_t index = 0;

    for (index = 0; index < RTC_BKP_DR_NUMBER; index++)
    {
        RTC_WriteBackupRegister(BKPDataReg[index], FirstBackupData + (index * 0x5A));
    }
}

/*******************************************************************************
* Function Name  : RTC_Configuration
* Description    : Configures the RTC.
* Input          : None
* Output         : None
* Return         : 0 reday,-1 error.
*******************************************************************************/
static int RTC_Configuration(void)
{
    volatile uint32_t count=0x200000;
    RTC_TimeTypeDef   RTC_TimeStructure;
    RTC_DateTypeDef   RTC_DateStructure;
    RTC_InitTypeDef   RTC_InitStructure;

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* if not select the RTC Clock source, use default: RTC_CLOCK_SOURCE_LSI */
#if (!defined(RTC_CLOCK_SOURCE_LSI)) && (!defined(RTC_CLOCK_SOURCE_LSE))
#define RTC_CLOCK_SOURCE_LSI
#endif

#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
    /* The RTC Clock may varies due to LSI frequency dispersion. */
    /* Enable the LSI OSC */
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */
    while((RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) && (count != 0))
    {
        count--;
    }
    if(count == 0)
    {
        return -1;
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */
    while((RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) && (count != 0))
    {
        count--;
    }
    if(count == 0)
    {
        return -1;
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    SynchPrediv = 0xFF;
    AsynchPrediv = 0x7F;

#endif /* RTC_CLOCK_SOURCE_LSI */

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* Write to the first RTC Backup Data Register */
    RTC_WriteBackupRegister(RTC_BKP_DR0, FIRST_DATA);

    /* Display the new RCC BDCR and RTC TAFCR Registers */
    rt_kprintf ("RTC Reconfig \n");
    rt_kprintf ("RCC BDCR = 0x%x\n", RCC->BDCR);
    rt_kprintf ("RTC TAFCR = 0x%x\n", RTC->TAFCR);

    /* Set the Time */
    RTC_TimeStructure.RTC_Hours   = 0x08;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00;

    /* Set the Date */
    RTC_DateStructure.RTC_Month = RTC_Month_March;
    RTC_DateStructure.RTC_Date = 0x18;
    RTC_DateStructure.RTC_Year = 0x11;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Friday;

    /* Calendar Configuration */
    RTC_InitStructure.RTC_AsynchPrediv = AsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv =  SynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    /* Set Current Time and Date */
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

//    /* Configure the RTC Wakeup Clock source and Counter (Wakeup event each 1 second) */
//    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
//    RTC_SetWakeUpCounter(0x7FF);

//    /* Enable the Wakeup Interrupt */
//    RTC_ITConfig(RTC_IT_WUT, ENABLE);
//
//    /* Enable Wakeup Counter */
//    RTC_WakeUpCmd(ENABLE);

    /*  Backup SRAM ***************************************************************/
    /* Enable BKPRAM Clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

    /* Write to Backup SRAM with 32-Bit Data */
    for (i = 0x0; i < 0x1000; i += 4)
    {
        *(__IO uint32_t *) (BKPSRAM_BASE + i) = i;
    }
    /* Check the written Data */
    for (i = 0x0; i < 0x1000; i += 4)
    {
        if ((*(__IO uint32_t *) (BKPSRAM_BASE + i)) != i)
        {
            errorindex++;
        }
    }

    if(errorindex)
    {
        rt_kprintf ("BKP SRAM Number of errors = %d\n", errorindex);
    }
    else
    {
        rt_kprintf ("BKP SRAM write OK \n");
    }

    /* Enable the Backup SRAM low power Regulator to retain it's content in VBAT mode */
    PWR_BackupRegulatorCmd(ENABLE);

    /* Wait until the Backup SRAM low power Regulator is ready */
    while(PWR_GetFlagStatus(PWR_FLAG_BRR) == RESET)
    {
    }

    /* RTC Backup Data Registers **************************************************/
    /* Write to RTC Backup Data Registers */
    WriteToBackupReg(FIRST_DATA);

    return 0;
}

void rt_hw_rtc_init(void)
{
    rtc.type	= RT_Device_Class_RTC;

    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != FIRST_DATA)
    {
        rt_kprintf("rtc is not configured\n");
        rt_kprintf("please configure with set_date and set_time\n");
        if (RTC_Configuration() != 0)
        {
            rt_kprintf("rtc configure fail...\r\n");
            return ;
        }
    }
    else
    {
        /* Enable the PWR APB1 Clock Interface */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);

        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
    }

    /* register rtc device */
    rtc.init 	= RT_NULL;
    rtc.open 	= rt_rtc_open;
    rtc.close	= RT_NULL;
    rtc.read 	= rt_rtc_read;
    rtc.write	= RT_NULL;
    rtc.control = rt_rtc_control;

    /* no private */
    rtc.user_data = RT_NULL;

    rt_device_register(&rtc, "rtc", RT_DEVICE_FLAG_RDWR);

#ifdef RT_USING_FINSH
    {
        extern void list_date(void);
        list_date();
    }
#endif

    return;
}
