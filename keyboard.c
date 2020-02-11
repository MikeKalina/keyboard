#include "keyboard.h"
#include "gpio.h"
#include "rtos.h"

T_PIN_NAME rowPins[] =
{
  KEY_ROW_1,
  KEY_ROW_2,
  KEY_ROW_3,
  KEY_ROW_4,
  KEY_ROW_5,
  KEY_ROW_6,
};

T_PIN_NAME colPins[] =
{
  KEY_COL_1,
  KEY_COL_2,
  KEY_COL_3,
};

#define KEY_ROW_MAX  (sizeof(rowPins)/sizeof(*rowPins))
#define KEY_COL_MAX  (sizeof(colPins)/sizeof(*colPins))

struct T_LONG_PRESS_DELAYS
{
  uint8_t pressNumBorder;
  uint32_t longPressTime;
} LongPressDelays[] =
{
  {5, 400 / (BTN_DELAY * KEY_ROW_MAX)},
  {15, 200 / (BTN_DELAY * KEY_ROW_MAX)},
  {100, 50 / (BTN_DELAY * KEY_ROW_MAX)}
};

void KeyboardListening(void)
{
  static uint32_t keyMask = 0, checkLongPressCntr = 0, longPressCntr = 0;
  static uint8_t longPressStep;
  uint32_t i, j;
  
  for(j = 0; j < KEY_ROW_MAX; j++)
  {
    PinSet(rowPins[j]);
    TaskDelay(BTN_DELAY);
    
    for(i = 0; i < KEY_COL_MAX; i++)
    {
      uint8_t pinNum = j * KEY_COL_MAX + i;
      if(PinGetInState(colPins[i]))
      {
        /* Если кнопка не была нажата */
        if(!(keyMask & (1 << pinNum)))
        {
          /* Задержка против дребезга */
          TaskDelay(BTN_DEBOUNCE_DELAY);
          /* Регистрация первого нажатия */
          if(PinGetInState(colPins[i]))
          {
            keyMask |= (1 << pinNum);
            checkLongPressCntr = 0;
            longPressCntr = 0;
            longPressStep = 0;
            vQueueTask1Send(&pinNum, false);
          }
        }
        /* Если кнопка была нажата */
        else
        {
          /* Регистрация долгого нажатия */
          if(++checkLongPressCntr >= LongPressDelays[longPressStep].longPressTime)
          {
            checkLongPressCntr = 0;
            if(++longPressCntr >= LongPressDelays[longPressStep].pressNumBorder
               && longPressStep < sizeof(LongPressDelays)/sizeof(*LongPressDelays) - 1)
              longPressStep++;
            vQueueTask1Send(&pinNum, false);
          }
        }
      }
      else
        keyMask &= ~(1 << pinNum);
    }
    PinReset(rowPins[j]);
  }
}