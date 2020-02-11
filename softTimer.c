#include "softTimer.h"

T_SOFT_TIMER_STRUCT softTimers[SOFT_TIMER_NUM] = {0};

bool SoftTimerStart(uint32_t id, uint32_t cntrMax, int32_t repeatNum, int32_t funcParam,
                    SoftTimerFuncPtr periodFunc, SoftTimerFuncPtr endFunc)
{
  if(id >= SOFT_TIMER_NUM)
    return false;
  
  softTimers[id].cntr = 0;
  softTimers[id].cntrMax = cntrMax;
  softTimers[id].storeMax = cntrMax;
  softTimers[id].repeatNum = repeatNum;
  softTimers[id].funcParam = funcParam;
  softTimers[id].periodFunc = periodFunc;
  softTimers[id].endFunc = endFunc;
  
  return true;
}

bool SoftTimerStop(uint32_t id)
{
  if(id >= SOFT_TIMER_NUM)
    return false;
  
  softTimers[id].cntrMax = 0;
  
  return true;
}

void SoftTimerStopAll(void)
{
  uint32_t i;
  for(i = 0; i < SOFT_TIMER_NUM; i++)
    softTimers[i].cntrMax = 0;
}

bool SoftTimerIsActive(uint32_t id)
{
  if(id >= SOFT_TIMER_NUM)
    return false;
  
  return (softTimers[id].cntrMax != 0);
}

uint32_t SoftTimerIncFunc(uint8_t *timerReady)
{
  static uint32_t i = 0;
  
  *timerReady = 0;
  
  if(softTimers[i].cntrMax && softTimers[i].cntr < softTimers[i].cntrMax)
  {
    if(++(softTimers[i].cntr) == softTimers[i].cntrMax)
    {
      softTimers[i].cntrMax = 0;
      *timerReady = 1;
    }
  }
  
  if(++i >= SOFT_TIMER_NUM)
    i = 0;
  
  return i;
}

void SoftTimerProcFunc(uint32_t id)
{
  /* Бесконеный таймер */
  if(softTimers[id].repeatNum < 0)
  {
    /* Восстанавливаем работу таймера */
    softTimers[id].cntr = 0;
    softTimers[id].cntrMax = softTimers[id].storeMax;
    /* Вызываем обработчик */
    if(softTimers[id].endFunc)
      softTimers[id].endFunc(softTimers[id].funcParam);
  }
  /* Конечный таймер с несколькими вызовами */
  else if(softTimers[id].repeatNum)
  {
    /* Если остались вызовы */
    if(--(softTimers[id].repeatNum))
    {
      /* Восстанавливаем работу таймера */
      softTimers[id].cntr = 0;
      softTimers[id].cntrMax = softTimers[id].storeMax;
      /* Вызываем обработчик */
      if(softTimers[id].periodFunc)
        softTimers[id].periodFunc(softTimers[id].funcParam);
    }
    else
    {
      /* Вызываем обработчик */
      if(softTimers[id].endFunc)
        softTimers[id].endFunc(softTimers[id].funcParam);
    }
  }
  /* Одиночный таймер */
  else
  {
    /* Вызываем обработчик */
    if(softTimers[id].endFunc)
      softTimers[id].endFunc(softTimers[id].funcParam);
  }
}
