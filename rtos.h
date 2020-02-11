#ifndef RTOS_H_
#define RTOS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "softTimer.h"
#include "pwm.h"

#define TASK_NUM                1
#define PER_TASK_NUM            1
#define QUEUE_TASK_NUM          3
#define QUEUE_NUM               0
#define TIMER_NUM               1
#define MUTEX_NUM               1

/* ================================= TASKS ================================= */
#if TASK_NUM > 0
#define TASK_1_PRIORITY         1
#define TASK_1_STACK            2
void vTask1(void *pvParameters);
void Task1Func(void);
#endif

/* ============================ PERIODIC TASKS ============================ */
#if PER_TASK_NUM > 0
#define PER_TASK_1_PERIOD       500
#define PER_TASK_1_PRIORITY     1
#define PER_TASK_1_STACK        2
void vPerTask1(void *pvParameters);
void PerTask1Func(void);
#endif

#if PER_TASK_NUM > 1
#define PER_TASK_2_PERIOD       10
#define PER_TASK_2_PRIORITY     2
#define PER_TASK_2_STACK        2
void vPerTask2(void *pvParameters);
void PerTask2Func(void);
#endif

/* ============================== QUEUE TASKS ============================== */
#if QUEUE_TASK_NUM > 0
#define QUEUE_TASK_1_SIZE       10
#define QUEUE_TASK_1_ELEM_SIZE  1
#define QUEUE_TASK_1_PRIORITY   1
#define QUEUE_TASK_1_STACK      2
bool vQueueTask1Send(uint8_t *data, bool fromIsr);
void vQueueTask1(void *pvParameters);
void QueueTask1Func(int8_t data);
#endif

#if QUEUE_TASK_NUM > 1
#define QUEUE_TASK_2_SIZE       SOFT_TIMER_NUM
#define QUEUE_TASK_2_ELEM_SIZE  sizeof(uint32_t)
#define QUEUE_TASK_2_PRIORITY   2
#define QUEUE_TASK_2_STACK      2
bool vQueueTask2Send(uint8_t *data, bool fromIsr);
void vQueueTask2(void *pvParameters);
void QueueTask2Func(uint32_t data);
#endif

#if QUEUE_TASK_NUM > 2
#define QUEUE_TASK_3_SIZE       5
#define QUEUE_TASK_3_ELEM_SIZE  sizeof(T_PWM_QUEUE_ITEM)
#define QUEUE_TASK_3_PRIORITY   1
#define QUEUE_TASK_3_STACK      2
bool vQueueTask3Send(uint8_t *data, bool fromIsr);
void vQueueTask3(void *pvParameters);
void QueueTask3Func(T_PWM_QUEUE_ITEM data);
#endif

/* ================================= QUEUE ================================= */
#if QUEUE_NUM > 0
#define QUEUE_1_SIZE            1
#define QUEUE_1_ELEM_SIZE       2
bool vQueue1Send(uint8_t *data, bool fromIsr);
bool vQueue1Receive(uint8_t *data);
uint32_t vQueue1GetSize();
#endif

/* ================================= TIMER ================================= */
#if TIMER_NUM > 0
#define TIMER_1_PERIOD          1000
#define TIMER_1_IS_PERIODICAL   1
#define TIMER_1_ID              0
#define TIMER_1_START           1
void vTimer1Callback(xTimerHandle xTimer);
bool vTimer1Start();
void vTimer1StartIsr();
bool vTimer1Stop();
void vTimer1StopIsr();
bool vTimer1SetPeriod(uint32_t timerPeriod);
void vTimer1SetPeriodIsr(uint32_t timerPeriod);
#endif

/* ================================= MUTEX ================================= */
#if MUTEX_NUM > 0
void vMutex1Take();
void vMutex1Give();
#endif

bool RtosInit();
void TaskDelay(uint32_t delay);

#ifdef CPU_LOAD_MONITOR
uint8_t get_cpu_load();
#endif

#endif