#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>
#include "rtos.h"

#define BTN_DELAY                       2
#define BTN_DEBOUNCE_DELAY              20

typedef enum
{
  BUT_L = 0,
  BUT_UP,
  BUT_R,
  BUT_MENU,
  BUT_DOWN,
  BUT_MENU1,
  BUT_1,
  BUT_2,
  BUT_3,
  BUT_4,
  BUT_5,
  BUT_6,
  BUT_7,
  BUT_8,
  BUT_9,
  BUT_V,
  BUT_0,
  BUT_X,
  BUT_NUM
} T_BUTTONS_NAME;

void KeyboardListening(void);

#endif /* KEYBOARD_H_ */