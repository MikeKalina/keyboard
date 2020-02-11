#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint8_t pwmNum;
  uint32_t pwmPeriod;
  uint32_t pwmDutyCycle;
} T_PWM_QUEUE_ITEM;

void PwmProcFunc(T_PWM_QUEUE_ITEM pwmItem);

#endif /* PWM_H_ */