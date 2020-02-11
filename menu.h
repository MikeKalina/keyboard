#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include <stdbool.h>
#include "keyboard.h"

#include "dogs_disp_driver.h"

#define CALL_SHOW_ID    (BUT_NUM)
#define CALL_TIMR_ID    (BUT_NUM + 1)

#define START_MENU_RAW  (1)
#define END_MENU_ROW    (3)

#define MENU_DYNAMIC_SHORT_TIMEOUT      300
#define MENU_DYNAMIC_LONG_TIMEOUT       1000

typedef enum
{
  STATE_MAIN = 0,
  STATE_MENU,
  STATE_MPU_RAW,
  STATE_SNAKE,
  STATE_RGB_LED,
  
  STATE_MAX
} T_MENU_STATE;

/*! ���, ������������ ��������� �� �������-���������� ������ ���� */
typedef void (*FuncPtr)(int32_t);

/*! ���������, ����������� ����� ���� */
typedef struct
{
  /*! ��������� �� ��������� ����� */
  void *Next;
  
  /*! ��������� �� ���������� ����� */
  void *Previous;
  
  /*! ��������� �� ������������ ���� */
  void *Parent;
  
  /*! ��������� �� ������� */
  void *Child;
  
  /*! ��������� �� ������� ���������� ������ ���� */
  T_MENU_STATE FuncState;
  
  /*! �������������, ������������ � ������� */
  int FuncParam;
  
  /*! ���� ������, ���������� ������ �������������� ���������� */
  bool Admin;
  
  /*! ��������� �� ������ ������ ������ */
  char *Text;
} T_MENU_ITEM;

#define MAKE_MENU(Name, Next, Previous, Parent, Child, FuncState, FuncParam, Admin, Text) \
  extern T_MENU_ITEM Next; \
  extern T_MENU_ITEM Previous; \
  extern T_MENU_ITEM Parent; \
  extern T_MENU_ITEM Child; \
  T_MENU_ITEM Name = {(void *)&Next, (void *)&Previous, (void *)&Parent, (void *)&Child, \
    (T_MENU_STATE)FuncState, (int)FuncParam, (bool)Admin, (char *)Text};
    
void MenuShow(void);
void MenuProc(int8_t id);

void MainFunc(int32_t id);
void MenuFunc(int32_t id);
void MpuRawFunc(int32_t id);
void SnakeFunc(int32_t id);
void RgbLedFunc(int32_t id);

#endif