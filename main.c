#include <stdlib.h>
#include "rtos.h"
#include "gpio.h"
#include "keyboard.h"
#include "uart.h"
#include "spi.h"
#include "timer.h"
#include "dogs_disp_driver.h"
#include "rtc.h"
#include "i2c.h"
#include "mpu6050.h"
#include "menu.h"
#include "softTimer.h"
#include "ws2812.h"
#include "pwm.h"

#define DEFAULT_BRIGHTNESS      50 // percent

bool rtosStartFlag = false;

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

int main()
{
  /* Init GPIO module */
  GpioInit();
  
  /* Init UART module */
  UartInit();
  
  /* Init UART module */
  SpiInit();
  
  /* Init UART module */
  I2cInit();
  
  /* Init TIMER module */
  TimerInit();
  
  /* Init display */
  DisplayInit(SPI1_Send);
  
  /* Init RTC */
  RtcInit();
  
  /* Init MPU */
//  MpuInit();
  
  /* Init MCO pin */
//  McoPinEnable();
  
  /* Set default brightness */
  Timer10SetCompare(DEFAULT_BRIGHTNESS * 10);
  /* Show main window */
  MenuProc(CALL_SHOW_ID);
  
  /* Инициализация генератора случайных чисел */
  srand(TimerGetCntr());
  
  /* Инициализация rgb */
  RgbLedInit(SPI2_Send);
  
  /* Init rtos objects */
  RtosInit();
  
  /* Start RTOS */
  rtosStartFlag = true;
  vTaskStartScheduler();
  
  return 0;
}

#if TASK_NUM > 0
void Task1Func(void)
{
  KeyboardListening();
}
#endif

#if PER_TASK_NUM > 0
void PerTask1Func(void)
{
  char str[SYMB_PER_LINE + 1];
  /* ====================== PWM INPUT ====================== */
//  sprintf(str, "%5d", Timer10GetCompare());
  
  /* ========================= RTC ========================= */
//  RTC_TimeTypeDef time;
//  RTC_DateTypeDef date;
//  RtcGetTime(&time);
//  RtcGetDate(&date);
//  sprintf(str, " %02d:%02d:%02d ", time.RTC_Hours, time.RTC_Minutes, time.RTC_Seconds);
//  DisplayWriteString(0, 2, str, false);
  
  /* ====================== CPU LOAD ======================= */
#ifdef CPU_LOAD_MONITOR
  uint8_t cpuLoad = get_cpu_load();
  sprintf(str, "%3d%%", cpuLoad);
  DisplayWriteString(8, 0, str, false);
#endif
}
#endif

#if PER_TASK_NUM > 1
void PerTask2Func()
{
//  static uint32_t calibCntr = MPU_CALIB_SAMPLES_NUM, showCntr = 0;
//  if(calibCntr)
//  {
//    MpuWriteGyrOffset(MPU_ADDRESS);
//    calibCntr--;
//    if(!calibCntr)
//      MpuSetGyrOffset(MPU_CALIB_SAMPLES_NUM);
//    else
//      DisplayWriteString(0, calibCntr * 3 / MPU_CALIB_SAMPLES_NUM, "Калибр. гир.", false);
//    return;
//  }
}
#endif

#if QUEUE_TASK_NUM > 0
void QueueTask1Func(int8_t data)
{
  MenuProc(data);
}
#endif

#if QUEUE_TASK_NUM > 1
void QueueTask2Func(uint32_t data)
{
  SoftTimerProcFunc(data);
}
#endif

#if QUEUE_TASK_NUM > 2
void QueueTask3Func(T_PWM_QUEUE_ITEM data)
{
  PwmProcFunc(data);
}
#endif

void BrightnessProc(uint8_t id)
{
  char str[SYMB_PER_LINE + 1];
  static uint32_t displayBrightness = DEFAULT_BRIGHTNESS;
  
  if(id == BUT_UP || id == BUT_DOWN)
  {
    if(id == BUT_UP)
    {
      if(displayBrightness < 99)
        displayBrightness += 1;
      else
        displayBrightness = 100;
    }
    else
    {
      if(displayBrightness > 1)
        displayBrightness -= 1;
      else
        displayBrightness = 0;
    }
    
    Timer10SetCompare(displayBrightness * 10);
    sprintf(str, "%3d%%", displayBrightness);
    DisplayWriteString(4, 0, str, false);
  }
}

void Mpu6050ShowRaw()
{
  char str[SYMB_PER_LINE + 1];
  T_VECTOR_16 acc, gyr;
    
  MpuReadAcc(MPU_ADDRESS);
  MpuReadGyr(MPU_ADDRESS);
  
  MpuGetAcc(&acc);
  MpuGetGyr(&gyr);
  sprintf(str, "%-5d  %-5d", acc.x, gyr.x);
  DisplayWriteString(0, 1, str, false);
  sprintf(str, "%-5d  %-5d", acc.y, gyr.y);
  DisplayWriteString(0, 2, str, false);
  sprintf(str, "%-5d  %-5d", acc.z, gyr.z);
  DisplayWriteString(0, 3, str, false);
  
  /* temp{ */
  static int8_t curPos = 0;
  if(acc.y < -10000)
  {
    if(++curPos >= 5)
      curPos = 0;
  }
  else if(acc.y > 10000)
  {
    if(--curPos < 0)
      curPos = 4;
  }
  uint8_t r = abs(acc.x) / 100;
  uint8_t g = abs(acc.y) / 100;
  uint8_t b = abs(acc.z) / 100;
  
  if(r < 10)
    r = 0;
  if(g < 10)
    g = 0;
  if(b < 10)
    b = 0;
  
  T_RGB_LED rgbLeds[3] = {0};
  rgbLeds[0].r = r;
  rgbLeds[1].g = g;
  rgbLeds[2].b = b;
  RgbLedWrite(rgbLeds, sizeof(rgbLeds) / sizeof(*rgbLeds));
  /* temp} */
}