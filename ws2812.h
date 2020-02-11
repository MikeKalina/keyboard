#ifndef WS2812_H_
#define WS2812_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define LEDS_NUM        5

#define RES_BIT_NUM     240
#define LED_BIT_NUM     4
#define SEND_BIT_NUM    (RES_BIT_NUM + LEDS_NUM * 24 * LED_BIT_NUM)

typedef void (*sendFuncType)(uint8_t*, uint16_t);

typedef struct
{
  uint8_t g;
  uint8_t r;
  uint8_t b;
} T_RGB_LED;

void RgbLedInit(sendFuncType func);
void RgbLedWrite(T_RGB_LED *rgbLeds, uint16_t ledNum);

#endif /* WS2812_H_ */