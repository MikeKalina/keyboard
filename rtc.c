#include "rtc.h"

bool RtcInit()
{
  uint32_t AsynchPrediv = 0, SynchPrediv = 0;
  
//  if (RTC_ReadBackupRegister(RTC_BKP_DR0) != RTC_BACKUP_REG_VALUE)
  {
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    /* Allow access to RTC */
    PWR_RTCAccessCmd(ENABLE);
    
    /*
    PREDIV_A - AsynchPrediv
    PREDIV_S - SynchPrediv
    
    fCK_APRE = fRTCCLK / (PREDIV_A + 1)
    fCK_SPRE = fRTCCLK / ((PREDIV_S + 1)*(PREDIV_A + 1))
    */
    
#ifdef USE_LSI
    
    /* Enable the LSI OSC */ 
    RCC_LSICmd(ENABLE);

    /* Wait till LSI is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){}

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    
    AsynchPrediv = 15;
    SynchPrediv = 2508;
    
#elif defined(USE_HSE)
    
    /* Wait till HSE is ready */ 
    while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET){}
    
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div16);
    
    AsynchPrediv = 99;
    SynchPrediv = 4999;
    
#endif
    
    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
    
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitTypeDef rtcInitStruct;
    rtcInitStruct.RTC_AsynchPrediv = AsynchPrediv;
    rtcInitStruct.RTC_HourFormat = RTC_HourFormat_24;
    rtcInitStruct.RTC_SynchPrediv = SynchPrediv;
    if(RTC_Init(&rtcInitStruct) == ERROR)
      return false;
    
    RTC_TimeTypeDef time = {.RTC_Hours = 0, .RTC_Minutes = 0};
    RTC_DateTypeDef date = {.RTC_Month = 10, .RTC_Date = 25, .RTC_Year = 18};
    if(RtcSetTime(time) && RtcSetDate(date))
    {
      /* Indicator for the RTC configuration */
      RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_BACKUP_REG_VALUE);
    }
    else
      return false;
  }
//  else
//  {
//    /* Enable the PWR clock */
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
//    
//    /* Allow access to RTC */
//    PWR_RTCAccessCmd(ENABLE);
//    
//    /* Wait for RTC APB registers synchronisation */
//    RTC_WaitForSynchro();
//  }
  
  return true;
}

bool RtcSetTime(RTC_TimeTypeDef time)
{
  if(RTC_SetTime(RTC_Format_BIN, &time) == ERROR)
    return false;
  return true;
}

bool RtcSetDate(RTC_DateTypeDef date)
{
  if(RTC_SetDate(RTC_Format_BIN, &date) == ERROR)
    return false;
  return true;
}

bool RtcGetTime(RTC_TimeTypeDef* time)
{
  RTC_GetTime(RTC_Format_BIN, time);
  return true;
}

bool RtcGetDate(RTC_DateTypeDef* date)
{
  RTC_GetDate(RTC_Format_BIN, date);
  return true;
}