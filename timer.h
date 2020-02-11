#ifndef TIMER_H
#define TIMER_H

#include "stm32l1xx.h"
#include <stdint.h>
#include <stdbool.h>

#define SYSTICK_ENABLE_
#define TIMER_6_ENABLE
#define TIMER_7_ENABLE
#define TIMER_9_ENABLE_
#define TIMER_10_ENABLE
#define TIMER_11_ENABLE_

void TimerInit();

#ifdef SYSTICK_ENABLE
SysTickInit();
#endif
#ifdef TIMER_6_ENABLE
void Timer6Init();
void Timer6Start();
void Timer6Stop();
#ifdef CPU_LOAD_MONITOR
uint32_t TimerGetCntr();
void TimerResetCntr();
#endif
#endif
#ifdef TIMER_7_ENABLE
void Timer7Init();
void Timer7Start();
void Timer7Stop();
#endif
#ifdef TIMER_9_ENABLE
void Timer9Init();
void Timer9Start();
void Timer9Stop();
#endif
#ifdef TIMER_10_ENABLE
void Timer10Init();
void Timer10SetCompare(uint32_t value);
uint32_t Timer10GetCompare();
#endif
#ifdef TIMER_11_ENABLE
void Timer11Init();
void Timer11SetCompare(uint32_t value);
#endif

#endif