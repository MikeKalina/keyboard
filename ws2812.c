#include "ws2812.h"

static uint8_t ledSendMass[SEND_BIT_NUM / 8];
static const uint8_t zeroMass[] = {1, 0, 0, 0};
static const uint8_t oneMass[] = {1, 1, 1, 0};

static sendFuncType sendFunc = NULL;

void RgbLedInit(sendFuncType func)
{
  sendFunc = func;
}

void RgbLedWrite(T_RGB_LED *rgbLeds, uint16_t ledNum)
{
  uint32_t iSendBit, iSendByte, iLedBit, iLedByte, iDigByte;
  uint8_t *curDigPtr, *ledMass = (uint8_t*)rgbLeds;
  if(ledNum > LEDS_NUM)
    ledNum = LEDS_NUM;
  
  iSendByte = RES_BIT_NUM / 8;
  iSendBit = 7 - RES_BIT_NUM % 8;
  memset(ledSendMass, 0, sizeof(ledSendMass));
  for(iLedByte = 0; iLedByte < ledNum * 3; iLedByte++)
  {
    iLedBit = 8;
    while(iLedBit)
    {
      iLedBit--;
      if(ledMass[iLedByte] & (1 << iLedBit))
        curDigPtr = (uint8_t*)oneMass;
      else
        curDigPtr = (uint8_t*)zeroMass;
      
      for(iDigByte = 0; iDigByte < LED_BIT_NUM; iDigByte++)
      {
        if(curDigPtr[iDigByte])
          ledSendMass[iSendByte] |= 1 << iSendBit;
        if(!iSendBit)
        {
          iSendByte++;
          iSendBit = 7;
        }
        else
          iSendBit--;
      }
    }
  }
  
  if(iSendBit)
    iSendByte++;
  
  if(sendFunc)
    sendFunc(ledSendMass, iSendByte);
}