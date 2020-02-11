#include "timer.h"
#include "gpio.h"
#include "softTimer.h"
#include "rtos.h"
#include "pwm.h"

#ifdef CPU_LOAD_MONITOR
static uint32_t timerCntr = 0;
#endif

void TimerInit()
{
#ifdef SYSTICK_ENABLE
  SysTickInit();
#endif
#ifdef TIMER_6_ENABLE
  Timer6Init();
  Timer6Start();
#endif
#ifdef TIMER_7_ENABLE
  Timer7Init();
  Timer7Start();
#endif
#ifdef TIMER_9_ENABLE
  Timer9Init();
#endif
#ifdef TIMER_10_ENABLE
  Timer10Init();
#endif
#ifdef TIMER_11_ENABLE
  Timer11Init();
#endif
}

#ifdef SYSTICK_ENABLE
void SysTickInit()
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}
#endif

#ifdef TIMER_6_ENABLE
void Timer6Init()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  
  TIM_TimeBaseInitTypeDef timeStruct;
  timeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  timeStruct.TIM_CounterMode = TIM_CounterMode_Up;
  timeStruct.TIM_Period = 1;
  timeStruct.TIM_Prescaler = 1600 - 1;
  
  TIM_TimeBaseInit(TIM6, &timeStruct);
  TIM_ITConfig(TIM6, TIM_DIER_UIE, ENABLE);
  
  NVIC_SetPriority(TIM6_IRQn, 1); 
  NVIC_EnableIRQ(TIM6_IRQn);
}

void Timer6Start()
{
  TIM_Cmd(TIM6, ENABLE);
}

void Timer6Stop()
{
  TIM_Cmd(TIM6, DISABLE);
}

void TIM6_IRQHandler(void)
{
  TIM_ClearFlag(TIM6, TIM_SR_UIF);
#ifdef CPU_LOAD_MONITOR
  timerCntr++;
#endif
}

#ifdef CPU_LOAD_MONITOR
uint32_t TimerGetCntr()
{
  return timerCntr;
}
void TimerResetCntr()
{
  timerCntr = 0;
}
#endif
#endif

#ifdef TIMER_7_ENABLE
void Timer7Init()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  
  TIM_TimeBaseInitTypeDef timeStruct;
  timeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  timeStruct.TIM_CounterMode = TIM_CounterMode_Up;
  timeStruct.TIM_Period = 1;
  timeStruct.TIM_Prescaler = 16000 - 1;
  TIM_TimeBaseInit(TIM7, &timeStruct);
  
  TIM_ITConfig(TIM7, TIM_DIER_UIE, ENABLE);
  NVIC_SetPriority(TIM7_IRQn, 5); 
  NVIC_EnableIRQ(TIM7_IRQn);
}

void Timer7Start()
{
  TIM_Cmd(TIM7, ENABLE);
}

void Timer7Stop()
{
  TIM_Cmd(TIM7, DISABLE);
}

void TIM7_IRQHandler(void)
{
  TIM_ClearFlag(TIM7, TIM_SR_UIF);
  
  uint32_t timerId;
  uint8_t timerReady;
  do
  {
    timerId = SoftTimerIncFunc(&timerReady);
    if(timerReady)
    {
      vQueueTask2Send((uint8_t *)&timerId, true);
    }
  } while(timerId);
}
#endif

#ifdef TIMER_9_ENABLE
void Timer9Init()
{
  /* Настройка пина */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitTypeDef gpioStruct;
  gpioStruct.GPIO_Pin = GPIO_Pin_13;
  gpioStruct.GPIO_Mode = GPIO_Mode_AF;
  gpioStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &gpioStruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_TIM9);
  
  /* Настройка таймера */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
  TIM_TimeBaseInitTypeDef timeStruct;
  timeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  timeStruct.TIM_CounterMode = TIM_CounterMode_Up;
  timeStruct.TIM_Period = 0xFFFF;
  timeStruct.TIM_Prescaler = 31;// 1us
  TIM_TimeBaseInit(TIM9, &timeStruct);
  
    /* Настройка режима таймера */
  TIM_ICInitTypeDef icInitStruct;
  icInitStruct.TIM_Channel = TIM_Channel_1;
  icInitStruct.TIM_ICFilter = 0;
  icInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;
  icInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  icInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_PWMIConfig(TIM9, &icInitStruct);
  
  /* Выбираем источник для триггера: вход 1 */
  TIM_SelectInputTrigger(TIM9, TIM_TS_TI1FP1);
  /* По событию от триггера счётчик будет сбрасываться. */
  TIM_SelectSlaveMode(TIM9, TIM_SlaveMode_Reset);
  /* Включаем события от триггера */
  TIM_SelectMasterSlaveMode(TIM9, TIM_MasterSlaveMode_Enable);
  
  TIM_ITConfig(TIM9, TIM_IT_CC1, ENABLE);
  NVIC_SetPriority(TIM9_IRQn, 5);
  NVIC_EnableIRQ(TIM9_IRQn);
  
  TIM_Cmd(TIM9, ENABLE);
}

void Timer9Start()
{
  TIM_Cmd(TIM9, ENABLE);
}

void Timer9Stop()
{
  TIM_Cmd(TIM9, DISABLE);
}

void TIM9_IRQHandler(void)
{
  static T_PWM_QUEUE_ITEM pwmItem;
  
  if (TIM_GetITStatus(TIM9, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM9, TIM_IT_CC1);
    
    NVIC_DisableIRQ(TIM9_IRQn);
    pwmItem.pwmNum = 1;
    pwmItem.pwmPeriod = TIM_GetCapture1(TIM9);
    pwmItem.pwmDutyCycle = TIM_GetCapture2(TIM9);
    vQueueTask3Send((uint8_t*)&pwmItem, true);
    NVIC_EnableIRQ(TIM9_IRQn);
  }
}
#endif

#ifdef TIMER_10_ENABLE
void Timer10Init()
{
  /* Настройка пина */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  GPIO_InitTypeDef gpioStruct;
  gpioStruct.GPIO_Pin = GPIO_Pin_12;
  gpioStruct.GPIO_Mode = GPIO_Mode_AF;
  gpioStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &gpioStruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_TIM10);
  
  /* Настройка таймера */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
  TIM_TimeBaseInitTypeDef timeStruct;
  timeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  timeStruct.TIM_CounterMode = TIM_CounterMode_Up;
  timeStruct.TIM_Period = 1000;
  timeStruct.TIM_Prescaler = 31;// 1us
  TIM_TimeBaseInit(TIM10, &timeStruct);
  
  /* Настройка режима таймера */
  TIM_OCInitTypeDef ocInitStruct;
  ocInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  ocInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  ocInitStruct.TIM_Pulse = 0;
  ocInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM10, &ocInitStruct);
  
  TIM_Cmd(TIM10, ENABLE);
}

void Timer10SetCompare(uint32_t value)
{
  TIM_SetCompare1(TIM10, value);
}

uint32_t Timer10GetCompare()
{
  return TIM_GetCapture1(TIM10);
}
#endif

#ifdef TIMER_11_ENABLE
void Timer11Init()
{
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11, ENABLE);
  
  /* Настройка таймера */
  TIM_TimeBaseInitTypeDef timeStruct;
  timeStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  timeStruct.TIM_CounterMode = TIM_CounterMode_Up;
  timeStruct.TIM_Period = 1000;
  timeStruct.TIM_Prescaler = 31;// 1us
  TIM_TimeBaseInit(TIM11, &timeStruct);
  
  /* Настройка пина */
  GPIO_InitTypeDef gpioStruct;
  gpioStruct.GPIO_Pin = GPIO_Pin_15;
  gpioStruct.GPIO_Mode = GPIO_Mode_AF;
  gpioStruct.GPIO_OType = GPIO_OType_PP;
  gpioStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  gpioStruct.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_Init(GPIOB, &gpioStruct);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM11);
  
  /* Настройка режима таймера */
  TIM_OCInitTypeDef ocInitStruct;
  ocInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  ocInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  ocInitStruct.TIM_Pulse = 0;
  ocInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(TIM11, &ocInitStruct);
  
  TIM_Cmd(TIM11, ENABLE);
}

void Timer11SetCompare(uint32_t value)
{
  TIM_SetCompare1(TIM11, value);
}
#endif