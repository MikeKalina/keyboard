#include "rtos.h"
#include "timer.h"
#include "gpio.h"
#include "keyboard.h"

#if QUEUE_TASK_NUM > 0
volatile xQueueHandle xQueueTask1;
#endif

#if QUEUE_TASK_NUM > 1
volatile xQueueHandle xQueueTask2;
#endif

#if QUEUE_TASK_NUM > 2
volatile xQueueHandle xQueueTask3;
#endif

#if QUEUE_NUM > 0
volatile xQueueHandle xQueue1;
#endif

#if TIMER_NUM > 0
volatile xTimerHandle xTimer1;
#endif

#if MUTEX_NUM > 0
volatile xSemaphoreHandle xMutex;
#endif

#ifdef CPU_LOAD_MONITOR
volatile uint32_t last_idle_t;
volatile uint32_t idle_time;

void entry_task(char *taskName)
{
  if(!strcmp(taskName, "IDLE"))
  {
    last_idle_t = TimerGetCntr();
  }
}

void preceed_task(char *taskName)
{
  if(!strcmp(taskName, "IDLE"))
  {
    idle_time += TimerGetCntr() - last_idle_t;
  }
}

uint8_t get_cpu_load()
{
  uint32_t curRttVal = TimerGetCntr();
  uint8_t retVal = (100 * (curRttVal - idle_time))/curRttVal;
  idle_time = 0;
  TimerResetCntr();
  return retVal;
}
#endif

bool RtosInit()
{
#if TASK_NUM > 0
  if(xTaskCreate(vTask1, "task1", TASK_1_STACK * configMINIMAL_STACK_SIZE,
              NULL, TASK_1_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if PER_TASK_NUM > 0
  if(xTaskCreate(vPerTask1, "PerTask1", PER_TASK_1_STACK * configMINIMAL_STACK_SIZE,
              NULL, PER_TASK_1_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if PER_TASK_NUM > 1
  if(xTaskCreate(vPerTask2, "PerTask2", PER_TASK_2_STACK * configMINIMAL_STACK_SIZE,
              NULL, PER_TASK_2_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if QUEUE_TASK_NUM > 0
  xQueueTask1 = xQueueCreate(QUEUE_TASK_1_SIZE, QUEUE_TASK_1_ELEM_SIZE);
  if(xTaskCreate(vQueueTask1, "QueueTask1", QUEUE_TASK_1_STACK * configMINIMAL_STACK_SIZE,
                 NULL, QUEUE_TASK_1_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if QUEUE_TASK_NUM > 1
  xQueueTask2 = xQueueCreate(QUEUE_TASK_2_SIZE, QUEUE_TASK_2_ELEM_SIZE);
  if(xTaskCreate(vQueueTask2, "QueueTask2", QUEUE_TASK_2_STACK * configMINIMAL_STACK_SIZE,
                 NULL, QUEUE_TASK_2_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if QUEUE_TASK_NUM > 2
  xQueueTask3 = xQueueCreate(QUEUE_TASK_3_SIZE, QUEUE_TASK_3_ELEM_SIZE);
  if(xTaskCreate(vQueueTask3, "QueueTask3", QUEUE_TASK_3_STACK * configMINIMAL_STACK_SIZE,
                 NULL, QUEUE_TASK_3_PRIORITY, NULL) != pdPASS)
    return false;
#endif
  
#if QUEUE_NUM > 0
  xQueue1 = xQueueCreate(QUEUE_1_SIZE, QUEUE_1_ELEM_SIZE);
#endif
  
#if TIMER_NUM > 0
  xTimer1 = xTimerCreate("Timer1", TIMER_1_PERIOD / portTICK_RATE_MS, TIMER_1_IS_PERIODICAL,
                         TIMER_1_ID, vTimer1Callback);
  if(TIMER_1_START)
    xTimerReset(xTimer1, 0);
#endif
  
#if MUTEX_NUM > 0
  xMutex = xSemaphoreCreateMutex();
  if(xMutex == NULL)
    return false;
#endif
  
  return true;
}

void TaskDelay(uint32_t delay)
{
  vTaskDelay(delay / portTICK_PERIOD_MS);
}

/* ================================= TASKS ================================= */
#if TASK_NUM > 0
void vTask1(void *pvParameters)
{
  for (;;)
  {
    Task1Func();
  }
}
#endif

/* ============================ PERIODIC TASKS ============================ */
#if PER_TASK_NUM > 0
void vPerTask1(void *pvParameters)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    PerTask1Func();
    vTaskDelayUntil(&xLastWakeTime, (PER_TASK_1_PERIOD / portTICK_PERIOD_MS));
  }
}
#endif

#if PER_TASK_NUM > 1
void vPerTask2(void *pvParameters)
{
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  for (;;)
  {
    PerTask2Func();
    vTaskDelayUntil(&xLastWakeTime, (PER_TASK_2_PERIOD / portTICK_PERIOD_MS));
  }
}
#endif

/* ============================== QUEUE TASKS ============================== */
#if QUEUE_TASK_NUM > 0
bool vQueueTask1Send(uint8_t *data, bool fromIsr)
{
  if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    return false;
  
  if(fromIsr)
  {
    portBASE_TYPE pxHigherPriorityTaskWoken;
    if(xQueueSendFromISR(xQueueTask1, data, &pxHigherPriorityTaskWoken) != pdPASS)
      return false;
    if(pxHigherPriorityTaskWoken == pdTRUE)
      taskYIELD();
  }
  else
  {
    if(xQueueSend(xQueueTask1, data, 0) != pdPASS)
      return false;
  }
  return true;
}

void vQueueTask1(void *pvParameters)
{
  int8_t data;
  for (;;)
  {
    xQueueReceive(xQueueTask1, &data, portMAX_DELAY);
    QueueTask1Func(data);
  }
}
#endif

#if QUEUE_TASK_NUM > 1
bool vQueueTask2Send(uint8_t *data, bool fromIsr)
{
  if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    return false;
  
  if(fromIsr)
  {
    portBASE_TYPE pxHigherPriorityTaskWoken;
    if(xQueueSendFromISR(xQueueTask2, data, &pxHigherPriorityTaskWoken) != pdPASS)
      return false;
    if(pxHigherPriorityTaskWoken == pdTRUE)
      taskYIELD();
  }
  else
  {
    if(xQueueSend(xQueueTask2, data, 0) != pdPASS)
      return false;
  }
  return true;
}

void vQueueTask2(void *pvParameters)
{
  uint32_t data;
  for (;;)
  {
    xQueueReceive(xQueueTask2, &data, portMAX_DELAY);
    QueueTask2Func(data);
  }
}
#endif

#if QUEUE_TASK_NUM > 2
bool vQueueTask3Send(uint8_t *data, bool fromIsr)
{
  BaseType_t temp = xTaskGetSchedulerState();
  if(temp == taskSCHEDULER_NOT_STARTED)
    return false;
  
  if(fromIsr)
  {
    portBASE_TYPE pxHigherPriorityTaskWoken;
    if(xQueueSendFromISR(xQueueTask3, data, &pxHigherPriorityTaskWoken) != pdPASS)
      return false;
    if(pxHigherPriorityTaskWoken == pdTRUE)
      taskYIELD();
  }
  else
  {
    if(xQueueSend(xQueueTask3, data, 0) != pdPASS)
      return false;
  }
  return true;
}

void vQueueTask3(void *pvParameters)
{
  T_PWM_QUEUE_ITEM data;
  for (;;)
  {
    xQueueReceive(xQueueTask3, &data, portMAX_DELAY);
    QueueTask3Func(data);
  }
}
#endif

/* ================================= QUEUE ================================= */
#if QUEUE_NUM > 0
bool vQueue1Send(uint8_t *data, bool fromIsr)
{
  if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    return false;
  
  if(fromIsr)
  {
    portBASE_TYPE pxHigherPriorityTaskWoken;
    if(xQueueSendFromISR(xQueue1, data, &pxHigherPriorityTaskWoken) != pdPASS)
      return false;
    if(pxHigherPriorityTaskWoken == pdTRUE)
      taskYIELD();
  }
  else
  {
    if(xQueueSend(xQueue1, data, 0) != pdPASS)
      return false;
  }
  return true;
}

bool vQueue1Receive(uint8_t *data, uint32_t delay)
{
  if(xQueueReceive(xQueue1, data, delay))
    return true;
  return false;
}

uint32_t vQueue1GetSize()
{
  return uxQueueMessagesWaiting(xQueue1);
}
#endif

/* ================================= TIMER ================================= */
#if TIMER_NUM > 0
void vTimer1Callback(xTimerHandle xTimer)
{
  PinToggle(LED_3_PIN);
}

bool vTimer1Start()
{
  if(xTimerReset(xTimer1, 0) == pdPASS)
    return true;
  return false;
}

void vTimer1StartIsr()
{
  portBASE_TYPE pxHigherPriorityTaskWoken;
  xTimerResetFromISR(xTimer1, &pxHigherPriorityTaskWoken);
  if(pxHigherPriorityTaskWoken == pdTRUE)
    taskYIELD();
}

bool vTimer1Stop()
{
  if(xTimerStop(xTimer1, 0) == pdPASS)
    return true;
  return false;
}

void vTimer1StopIsr()
{
  portBASE_TYPE pxHigherPriorityTaskWoken;
  xTimerStopFromISR(xTimer1, &pxHigherPriorityTaskWoken);
  if(pxHigherPriorityTaskWoken == pdTRUE)
    taskYIELD();
}

bool vTimer1SetPeriod(uint32_t timerPeriod)
{
  if(xTimerChangePeriod(xTimer1, timerPeriod / portTICK_RATE_MS, 0) == pdPASS)
    return true;
  return false;
}

void vTimer1SetPeriodIsr(uint32_t timerPeriod)
{
  portBASE_TYPE pxHigherPriorityTaskWoken;
  xTimerChangePeriodFromISR(xTimer1, timerPeriod / portTICK_RATE_MS, &pxHigherPriorityTaskWoken);
  if(pxHigherPriorityTaskWoken == pdTRUE)
    taskYIELD();
}
#endif

#if MUTEX_NUM > 0
void vMutex1Take()
{
  xSemaphoreTake(xMutex, portMAX_DELAY);
}

void vMutex1Give()
{
  xSemaphoreGive(xMutex);
}
#endif

/* =============================== RTOS hooks =============================== */
void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

void vApplicationTickHook( void )
{
	/* This function will be called by each tick interrupt if
	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
	added here, but the tick hook is called from an interrupt context, so
	code must not attempt to block, and only the interrupt safe FreeRTOS API
	functions can be used (those that end in FromISR()). */
}

void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;

	/* Parameters are not used. */
	( void ) ulLine;
	( void ) pcFileName;

	taskENTER_CRITICAL();
	{
		while( ulSetToNonZeroInDebuggerToContinue == 0 )
		{
			/* Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
			non zero value to step out of this function to the point that raised
			this assert(). */
			__asm volatile( "NOP" );
			__asm volatile( "NOP" );
		}
	}
	taskEXIT_CRITICAL();
}