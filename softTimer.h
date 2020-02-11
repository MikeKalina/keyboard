#ifndef SOFT_TIMER_
#define SOFT_TIMER_

#include <stdint.h>
#include <stdbool.h>

typedef void (*SoftTimerFuncPtr)(int32_t);

typedef enum
{
  SOFT_TIMER_DYNAMIC = 0,
  SOFT_TIMER_NUM
} T_SOFT_TIMERS_ITEMS;

typedef struct
{
  uint32_t cntr;
  uint32_t cntrMax;
  uint32_t storeMax;
  int32_t repeatNum;
  int32_t funcParam;
  SoftTimerFuncPtr periodFunc;
  SoftTimerFuncPtr endFunc;
} T_SOFT_TIMER_STRUCT;

bool SoftTimerStart(uint32_t id, uint32_t cntrMax, int32_t repeatNum, int32_t funcParam,
                    SoftTimerFuncPtr periodFunc, SoftTimerFuncPtr endFunc);
bool SoftTimerStop(uint32_t id);
void SoftTimerStopAll(void);
bool SoftTimerIsActive(uint32_t id);

uint32_t SoftTimerIncFunc(uint8_t *timerReady);
void SoftTimerProcFunc(uint32_t id);

#endif