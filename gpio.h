#ifndef GPIO_H
#define GPIO_H

#include "stm32l1xx.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  LED_3_PIN = 0,
  LED_4_PIN,
  BUT_1_PIN,
  
  KEY_ROW_1,
  KEY_ROW_2,
  KEY_ROW_3,
  KEY_ROW_4,
  KEY_ROW_5,
  KEY_ROW_6,
  KEY_COL_1,
  KEY_COL_2,
  KEY_COL_3,
  
  DISP_CS,
  DISP_CD,
  DISP_RST,
  
  PIN_MAX// этот пункт всегда должен быть последним!
} T_PIN_NAME;

typedef struct
{
  T_PIN_NAME pinName;
  GPIO_TypeDef *port;
  uint16_t pin;
  uint32_t pinClk;
  uint8_t shift;
  GPIOMode_TypeDef mode;
} T_PIN_STRUCT;

extern T_PIN_STRUCT pin[PIN_MAX];

void GpioInit();

void PinSet(T_PIN_NAME);
void PinReset(T_PIN_NAME);
void PinWrite(T_PIN_NAME, uint8_t);
void PinToggle(T_PIN_NAME);

uint8_t PinGetOutState(T_PIN_NAME led);
uint8_t PinGetInState(T_PIN_NAME led);

void McoPinEnable();

#endif