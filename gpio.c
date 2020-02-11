#include "gpio.h"
#include "rtos.h"
#include "pwm.h"

void GpioInitPin(T_PIN_NAME);

T_PIN_STRUCT pin[PIN_MAX] = 
{
  {LED_3_PIN,   GPIOC,  GPIO_Pin_9,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource9,        GPIO_Mode_OUT},
  {LED_4_PIN,   GPIOC,  GPIO_Pin_8,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource8,        GPIO_Mode_OUT},
  {BUT_1_PIN,   GPIOA,  GPIO_Pin_0,     RCC_AHBPeriph_GPIOA,    GPIO_PinSource0,        GPIO_Mode_IN},
  
  {KEY_ROW_1,   GPIOC,  GPIO_Pin_0,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource0,        GPIO_Mode_OUT},
  {KEY_ROW_2,   GPIOC,  GPIO_Pin_1,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource1,        GPIO_Mode_OUT},
  {KEY_ROW_3,   GPIOC,  GPIO_Pin_2,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource2,        GPIO_Mode_OUT},
  {KEY_ROW_4,   GPIOC,  GPIO_Pin_3,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource3,        GPIO_Mode_OUT},
  {KEY_ROW_5,   GPIOC,  GPIO_Pin_4,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource4,        GPIO_Mode_OUT},
  {KEY_ROW_6,   GPIOC,  GPIO_Pin_5,     RCC_AHBPeriph_GPIOC,    GPIO_PinSource5,        GPIO_Mode_OUT},
  {KEY_COL_1,   GPIOC,  GPIO_Pin_10,    RCC_AHBPeriph_GPIOC,    GPIO_PinSource10,       GPIO_Mode_IN},
  {KEY_COL_2,   GPIOC,  GPIO_Pin_11,    RCC_AHBPeriph_GPIOC,    GPIO_PinSource11,       GPIO_Mode_IN},
  {KEY_COL_3,   GPIOC,  GPIO_Pin_12,    RCC_AHBPeriph_GPIOC,    GPIO_PinSource12,       GPIO_Mode_IN},
  
  {DISP_CS,     GPIOA,  GPIO_Pin_4,     RCC_AHBPeriph_GPIOA,    GPIO_PinSource4,        GPIO_Mode_OUT},
  {DISP_CD,     GPIOC,  GPIO_Pin_14,    RCC_AHBPeriph_GPIOC,    GPIO_PinSource14,       GPIO_Mode_OUT},
  {DISP_RST,    GPIOC,  GPIO_Pin_13,    RCC_AHBPeriph_GPIOC,    GPIO_PinSource13,       GPIO_Mode_OUT}, 
};

void GpioInit()
{
  // настройка пинов gpio
  int i;
  for(i = 0; i < PIN_MAX; i++)
  {
    GpioInitPin(pin[i].pinName);
  }
  
  /* настройка прерывания входных линий */
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  /* Connect EXTI0 Line to PA0 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  /* Configure EXTI0 line */
  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_SetPriority(EXTI0_IRQn, 5);
  NVIC_EnableIRQ(EXTI0_IRQn);
}

void GpioInitPin(T_PIN_NAME name)
{
  RCC_AHBPeriphClockCmd(pin[name].pinClk, ENABLE);
  
  GPIO_InitTypeDef initStruct;
  initStruct.GPIO_Mode = pin[name].mode;
  initStruct.GPIO_OType = GPIO_OType_PP;
  initStruct.GPIO_Pin = pin[name].pin;
  initStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  initStruct.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_Init(pin[name].port, &initStruct);
}

inline void PinSet(T_PIN_NAME name)
{
  GPIO_SetBits(pin[name].port, pin[name].pin);
}

inline void PinReset(T_PIN_NAME name)
{
  GPIO_ResetBits(pin[name].port, pin[name].pin);
}

inline void PinWrite(T_PIN_NAME name, uint8_t level)
{
  GPIO_WriteBit(pin[name].port, pin[name].pin, (BitAction)level);
}

inline void PinToggle(T_PIN_NAME name)
{
  GPIO_ToggleBits(pin[name].port, pin[name].pin);
}

inline uint8_t PinGetOutState(T_PIN_NAME name)
{
  return GPIO_ReadOutputDataBit(pin[name].port, pin[name].pin);
}

inline uint8_t PinGetInState(T_PIN_NAME name)
{
  return GPIO_ReadInputDataBit(pin[name].port, pin[name].pin);
}

void McoPinEnable()
{
  RCC_MCOConfig(RCC_MCOSource_LSI, RCC_MCODiv_16);
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/* BUTTON_1_PIN */
void EXTI0_IRQHandler()
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line0);
    
    T_PWM_QUEUE_ITEM pwmItem;
    pwmItem.pwmNum = 1;
    pwmItem.pwmPeriod = 1;
    pwmItem.pwmDutyCycle = 2000;
    vQueueTask3Send((uint8_t*)&pwmItem, true);
  }
}