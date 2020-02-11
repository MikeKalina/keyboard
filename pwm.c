#include "pwm.h"

void PwmProcFunc(T_PWM_QUEUE_ITEM pwmItem)
{
  extern void BrightnessProc(uint8_t id);
  if(pwmItem.pwmDutyCycle > 1600)
    BrightnessProc(1);
  else if(pwmItem.pwmDutyCycle < 1400)
    BrightnessProc(4);
}