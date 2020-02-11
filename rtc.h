#ifndef RTC_H_
#define RTC_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32l1xx.h"

#define USE_HSE_
#define USE_LSI

#define RTC_BACKUP_REG_VALUE    0x32F2

bool RtcInit();
bool RtcSetTime(RTC_TimeTypeDef);
bool RtcSetDate(RTC_DateTypeDef);
bool RtcGetTime(RTC_TimeTypeDef*);
bool RtcGetDate(RTC_DateTypeDef*);
#endif /* RTC_H_ */