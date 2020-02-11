#include <stdlib.h>

#include "menu.h"
#include "dogs_disp_driver.h"
#include "softTimer.h"
#include "ws2812.h"

/* "Пустой" пункт меню */
T_MENU_ITEM null_Menu = { NULL, NULL, NULL, NULL, NULL, 0, false, ""};

/* Name, Next, Previous, Parent, Child, FuncState, FuncParam, Admin, Text */
MAKE_MENU(main_window,  null_Menu,      null_Menu,      null_Menu,      menu_snake,     NULL,           0,              false,  "");

MAKE_MENU(menu_snake,   menu_mpu,       null_Menu,      main_window,    null_Menu,      STATE_SNAKE,    CALL_SHOW_ID,   false,  "Змейка");
MAKE_MENU(menu_mpu,     menu_led,       menu_snake,     main_window,    null_Menu,      STATE_MPU_RAW,  CALL_SHOW_ID,   false,  "MPU6050 raw show");
MAKE_MENU(menu_led,     null_Menu,      menu_mpu,       main_window,    menu_led_1,     NULL,           0,              false,  "RGB leds");

MAKE_MENU(menu_led_1,   menu_led_2,     null_Menu,      menu_led,       null_Menu,      STATE_RGB_LED,  -1,             false,  "Бегущая точка");
MAKE_MENU(menu_led_2,   menu_led_3,     menu_led_1,     menu_led,       null_Menu,      STATE_RGB_LED,  -2,             false,  "Случайная точка");
MAKE_MENU(menu_led_3,   null_Menu,      menu_led_2,     menu_led,       null_Menu,      STATE_RGB_LED,  -3,             false,  "Все цвета");

T_MENU_STATE curMenuState = STATE_MAIN;
/* Глобальная переменная указатель на текущий пункт меню */
T_MENU_ITEM *curMenuItem = &main_window;
static uint8_t activeMenuInd = 1;

FuncPtr menuFuncs[STATE_MAX] =
{
  MainFunc,
  MenuFunc,
  MpuRawFunc,
  SnakeFunc,
  RgbLedFunc,
};

void MenuNext(void);
void MenuPrev(void);
void MenuParent(void);
void MenuChild(void);
void MenuCallFunc(void);
void MenuFromFunc(void);
void MenuChange(T_MENU_ITEM *newMenu);
void DisplayMenuString(uint8_t row, char *strPtr, bool isInvert);
void DisplayActiveMenuString(int32_t callId);

extern void BrightnessProc(uint8_t id);

/* ================== Функция вызова обработчиков ================== */
void MenuProc(int8_t id)
{
  FuncPtr curFunc = menuFuncs[curMenuState];
  if(curFunc)
    curFunc(id);
}

/* ================== Функции навигации меню ================== */
void MenuNext(void)
{
  T_MENU_ITEM *newMenu = curMenuItem->Next;
  if(newMenu != &null_Menu)
  {
    if(activeMenuInd < END_MENU_ROW)
      activeMenuInd++;
    MenuChange(newMenu);
  }
}

void MenuPrev(void)
{
  T_MENU_ITEM *newMenu = curMenuItem->Previous;
  if(newMenu != &null_Menu)
  {
    if(activeMenuInd > START_MENU_RAW)
      activeMenuInd--;
    MenuChange(newMenu);
  }
}

void MenuParent(void)
{
  T_MENU_ITEM *newMenu = curMenuItem->Parent;
  if(newMenu == &main_window)
  {
    SoftTimerStop(SOFT_TIMER_DYNAMIC);
    curMenuState = STATE_MAIN;
    curMenuItem = newMenu;
    MainFunc(CALL_SHOW_ID);
  }
  else
  {
    activeMenuInd = 1;
    MenuChange(newMenu);
  }
}

void MenuChild(void)
{
  T_MENU_ITEM *newMenu = curMenuItem->Child;
  if(curMenuItem->FuncState > STATE_MAIN && curMenuItem->FuncState < STATE_MAX)
  {
    SoftTimerStop(SOFT_TIMER_DYNAMIC);
    MenuCallFunc();
  }
  else if(newMenu != &null_Menu)
  {
    if(curMenuState == STATE_MAIN)
      curMenuState = STATE_MENU;
    activeMenuInd = 1;
    MenuChange(newMenu);
  }
}

void MenuCallFunc(void)
{
  FuncPtr funcPtr = menuFuncs[curMenuItem->FuncState];
  int funcParam = curMenuItem->FuncParam;
  
  if(funcPtr)
  {
    curMenuState = curMenuItem->FuncState;
    funcPtr(funcParam);
  }
}

void MenuFromFunc()
{
  curMenuState = STATE_MENU;
  MenuShow();
}

void MenuChange(T_MENU_ITEM *newMenu)
{
  if(newMenu != &null_Menu)
  {
    /* Назначение нового пункта меню текущим */
    curMenuItem = newMenu;
    /* Вызов функции отрисовки текущего пункта меню на экране */
    MenuShow();
  }
}

static void DisplayMenuString(uint8_t row, char *strPtr, bool isInvert)
{
  static char str[SYMB_PER_LINE + 1];
  uint32_t len = strlen(strPtr);
  
  memset(str, ' ', SYMB_PER_LINE);
  str[SYMB_PER_LINE] = 0;
  
  if(len > SYMB_PER_LINE)
    len = SYMB_PER_LINE;
  memcpy(str, strPtr, len);
  
  DisplayWriteString(0, row, str, isInvert);
}

static void DisplayActiveMenuString(int32_t callId)
{
  static uint32_t dynamCntr = 0;
  char *str = curMenuItem->Text;
  
  /* Если вызов по таймеру очередной раз */
  if(callId)
  {
    str = &str[++dynamCntr];
    if(strlen(str) > SYMB_PER_LINE)
      SoftTimerStart(SOFT_TIMER_DYNAMIC, MENU_DYNAMIC_SHORT_TIMEOUT, 0, 1, NULL, DisplayActiveMenuString);
    else
      SoftTimerStart(SOFT_TIMER_DYNAMIC, MENU_DYNAMIC_LONG_TIMEOUT, 0, 0, NULL, DisplayActiveMenuString);
  }
  /* Если первый вызов */
  else
  {
    dynamCntr = 0;
    if(strlen(str) > SYMB_PER_LINE)
      SoftTimerStart(SOFT_TIMER_DYNAMIC, MENU_DYNAMIC_LONG_TIMEOUT, 0, 1, NULL, DisplayActiveMenuString);
    else
      SoftTimerStop(SOFT_TIMER_DYNAMIC);
  }
  DisplayMenuString(activeMenuInd, str, true);
}

void MenuShow(void)
{
  uint8_t tempRow;
  
  DisplayActiveMenuString(0);
  if(activeMenuInd > START_MENU_RAW)
  {
    tempRow = activeMenuInd - 1;
    T_MENU_ITEM *prevMenu = curMenuItem->Previous;
    while(tempRow >= START_MENU_RAW)
    {
      DisplayMenuString(tempRow, prevMenu->Text, false);
      tempRow--;
      prevMenu = prevMenu->Previous;
    }
  }
  if(activeMenuInd < END_MENU_ROW)
  {
    tempRow = activeMenuInd + 1;
    T_MENU_ITEM *nextMenu = curMenuItem->Next;
    while(tempRow <= END_MENU_ROW)
    {
      if(nextMenu != &null_Menu)
      {
        DisplayMenuString(tempRow, nextMenu->Text, false);
        nextMenu = nextMenu->Next;
      }
      else
      {
        DisplayMenuString(tempRow, "            ", false);
      }
      tempRow++;
    }
  }
}

/* ================== Функции-обработчики меню ================== */
void MainFunc(int32_t id)
{
  switch(id)
  {
  case CALL_SHOW_ID:
    DisplayWriteString(0, 1, "Главное окно", false);
    DisplayWriteString(0, 2, "            ", false);
    DisplayWriteString(0, 3, "            ", false);
    break;
  case BUT_MENU:
    MenuChild();
    break;
  case BUT_UP:
  case BUT_DOWN:
    BrightnessProc(id);
    break;
  default:
    break;
  }
}

void MenuFunc(int32_t id)
{
  switch(id)
  {
  case CALL_SHOW_ID:
    MenuShow();
    break;
  case BUT_UP:
    MenuPrev();
    break;
  case BUT_DOWN:
    MenuNext();
    break;
  case BUT_V:
    MenuChild();
    break;
  case BUT_X:
    MenuParent();
    break;
  default:
    break;
  }
}

void MpuRawFunc(int32_t id)
{
  extern void Mpu6050ShowRaw();
  
  switch(id)
  {
  case BUT_X:
    SoftTimerStop(SOFT_TIMER_DYNAMIC);
    MenuFromFunc();
    return;
  case BUT_V:
  case CALL_SHOW_ID:
    Mpu6050ShowRaw();
    SoftTimerStart(SOFT_TIMER_DYNAMIC, 100, 0, CALL_SHOW_ID, NULL, MpuRawFunc);
    break;
  default:
    break;
  }
}

void SnakeFunc(int32_t id)
{
  static const uint8_t fieldWidth = 102;
  static const uint8_t fieldHeight = 6 * 8;
  static const uint8_t startShiftX = 5;
  static const uint8_t startShiftY = 5;
  static const uint16_t minTimeout = 100;       //ms
  static const uint8_t maxSpeed = 5;
  
  enum{UP, RIGHT, DOWN, LEFT};
  enum{PLAY, PAUSE, END_GAME, START, SPEED, DEMO};
  typedef struct
  {
    uint16_t x;
    uint16_t y;
  }T_POINT_STRUCT;
  
  static T_POINT_STRUCT snakeHead;
  static T_POINT_STRUCT snakeTail;
  static T_POINT_STRUCT apple;
  static uint8_t frame[102 * 6] = {0};
  
  static uint8_t snakeDirRing[102 * 6 / 4] = {0};
  static uint32_t snakeDirPut = 0, snakeDirGet = 0;
  
  static uint32_t speed = 4;
  static uint32_t score;
  static uint8_t curDir, choiceDir;
  
  static bool accelSnake = false;
  static bool demoMode = false;
  static uint8_t playState;
  
  uint8_t i;
  
  switch(id)
  {
  case CALL_SHOW_ID:
    {
      SoftTimerStop(SOFT_TIMER_DYNAMIC);
      /* Сброс переменных */
      score = 0;
      accelSnake = false;
      playState = START;
      snakeDirPut = 0;
      snakeDirGet = 0;
      /* Выбор направления */
      rand();
      curDir = rand() % 4;
      choiceDir = curDir;
      /* Отрисовка окна */
      memset(frame, 0, sizeof(frame));
      for(i = 0; i < fieldHeight / 8; i++)
      {
        frame[i * fieldWidth] = 0xFF;
        frame[(i + 1) * fieldWidth - 1] = 0xFF;
      }
      for(i = 1; i < fieldWidth - 1; i++)
      {
        frame[i] = 0x01;
        frame[(fieldHeight / 8 - 1) * fieldWidth + i] = 0x80;
      }
      /* Отрисовка змейки */
      snakeHead.x = startShiftX + rand() % (fieldWidth - 2 * startShiftX);
      snakeHead.y = startShiftY + rand() % (fieldHeight - 2 * startShiftY);
      snakeTail = snakeHead;
      /* Отрисовка яблока */
      do
      {
        apple.x = 1 + rand() % (fieldWidth - 2);
        apple.y = 1 + rand() % (fieldHeight - 2);
      }
      while(snakeHead.x == apple.x && snakeHead.y == apple.y);
    }
    break;
  case BUT_UP:
    if(playState == PLAY)
    {
      if(curDir == UP)
        accelSnake = true;
      else
      {
        if(curDir != DOWN)
          choiceDir = UP;
        accelSnake = false;
      }
    }
    else if(playState != START)
    {
      playState--;
    }
    break;
  case BUT_DOWN:
    if(playState == PLAY)
    {
      if(curDir == DOWN)
        accelSnake = true;
      else
      {
        if(curDir != UP)
          choiceDir = DOWN;
        accelSnake = false;
      }
    }
    else if(playState != DEMO)
    {
      playState++;
    }
    break;
  case BUT_R:
    if(playState == PLAY)
    {
      if(curDir == RIGHT)
        accelSnake = true;
      else
      {
        if(curDir != LEFT)
          choiceDir = RIGHT;
        accelSnake = false;
      }
    }
    else if(playState == SPEED)
    {
      if(speed < maxSpeed)
        speed++;
    }
    else if(playState == DEMO)
    {
      demoMode = !demoMode;
    }
    break;
  case BUT_L:
    if(playState == PLAY)
    {
      if(curDir == LEFT)
        accelSnake = true;
      else
      {
        if(curDir != RIGHT)
          choiceDir = LEFT;
        accelSnake = false;
      }
    }
    else if(playState == SPEED)
    {
      if(speed > 1)
        speed--;
    }
    else if(playState == DEMO)
    {
      demoMode = !demoMode;
    }
    break;
  case BUT_X:
    {
      if(playState == PLAY || playState == PAUSE || playState == END_GAME)
      {
        SnakeFunc(CALL_SHOW_ID);
      }
      else
      {
        MenuFromFunc();
      }
    }
    return;
  case BUT_V:
    {
      if(playState == PLAY)
      {
        playState = PAUSE;
        SoftTimerStop(SOFT_TIMER_DYNAMIC);
      }
      else if(playState == PAUSE)
      {
        playState = PLAY;
        /* Перезапуск таймера */
        DisplayDrawFrame(frame);
        SoftTimerStart(SOFT_TIMER_DYNAMIC, minTimeout * (maxSpeed - speed + 1),
                       0, CALL_TIMR_ID, NULL, SnakeFunc);
      }
      else if(playState == START)
      {
        playState = PLAY;
        DisplayDrawFrame(frame);
        DisplayDrawPixel(frame, snakeHead.x, snakeHead.y, 1);
        DisplayDrawPixel(frame, apple.x, apple.y, 1);
        /* Запуск таймера */
        SoftTimerStart(SOFT_TIMER_DYNAMIC, minTimeout * (maxSpeed - speed + 1),
                       0, CALL_TIMR_ID, NULL, SnakeFunc);
      }
      else if(playState == END_GAME)
      {
        SnakeFunc(CALL_SHOW_ID);
      }
    }
    break;
  case CALL_TIMR_ID:
    {
      /* Поворот согласно управлению */
      if(demoMode)
      {
        //        bool findDirFlag = false;
        //        int16_t deltaX = apple.x - snakeHead.x;
        //        int16_t deltaY = apple.y - snakeHead.y;
        //        while(findDirFlag)
        //        {
        //        }
      }
      else
        curDir = choiceDir;
      snakeDirRing[snakeDirPut / 4] &= ~(0x03 << (2 * (snakeDirPut % 4)));
      snakeDirRing[snakeDirPut / 4] |= curDir << (2 * (snakeDirPut % 4));
      if(++snakeDirPut >= sizeof(snakeDirRing) * 4)
        snakeDirPut = 0;
      switch(curDir)
      {
      case UP:
        snakeHead.y--;
        break;
      case DOWN:
        snakeHead.y++;
        break;
      case RIGHT:
        snakeHead.x++;
        break;
      case LEFT:
        snakeHead.x--;
        break;
      default:
        break;
      }
      /* Проверка на съедание яблока */
      if(snakeHead.x == apple.x && snakeHead.y == apple.y)
      {
        score++;
        /* Отрисовка нового яблока */
        do
        {
          apple.x = 1 + rand() % (fieldWidth - 2);
          apple.y = 1 + rand() % (fieldHeight - 2);
        }
        while(DisplayIsPixelDraw(frame, apple.x, apple.y));
        DisplayDrawPixel(frame, apple.x, apple.y, 1);
      }
      /* Проверка новой точки на столкновение */
      else if(!DisplayIsPixelDraw(frame, snakeHead.x, snakeHead.y))
      {
        /* Закрашивание новой головы */
        DisplayDrawPixel(frame, snakeHead.x, snakeHead.y, 1);
        /* Стирание хвоста с экрана */
        DisplayDrawPixel(frame, snakeTail.x, snakeTail.y, 0);
        uint8_t nextTailDir = (snakeDirRing[snakeDirGet / 4] >> (2 * (snakeDirGet % 4))) & 0x03;
        if(++snakeDirGet >= sizeof(snakeDirRing) * 4)
          snakeDirGet = 0;
        switch(nextTailDir)
        {
        case UP:
          snakeTail.y--;
          break;
        case DOWN:
          snakeTail.y++;
          break;
        case RIGHT:
          snakeTail.x++;
          break;
        case LEFT:
          snakeTail.x--;
          break;
        default:
          break;
        }
      }
      else
      {
        /* Проигрыш */
        playState = END_GAME;
        char str[SYMB_PER_LINE + 1];
        sprintf(str, " %10d ", score);
        DisplayWriteString(0, 1, " GAME OVER! ", false);
        DisplayWriteString(0, 2, "   SCORE:   ", false);
        DisplayWriteString(0, 3, str, false);
        return;
      }
      /* Перезапуск таймера */
      uint32_t curTimeout = accelSnake ? minTimeout / 5 : minTimeout;
      SoftTimerStart(SOFT_TIMER_DYNAMIC, curTimeout * (maxSpeed - speed + 1),
                     0, CALL_TIMR_ID, NULL, SnakeFunc);
    }
    break;
  default:
    break;
  }
  if(playState == PAUSE)
  {
    char str[SYMB_PER_LINE + 1];
    DisplayWriteString(0, 1, "Продолжить? ", true);
    sprintf(str, " %10d ", score);
    DisplayWriteString(0, 2, "   SCORE:   ", false);
    DisplayWriteString(0, 3, str, false);
  }
  else if(playState >= START && playState <= DEMO)
  {
    char str[SYMB_PER_LINE + 1];
    sprintf(str, " Скорость:%1d ", speed);
    DisplayWriteString(0, 1, "   Старт!   ", playState == START);
    DisplayWriteString(0, 2, str, playState == SPEED);
    if(demoMode)
      DisplayWriteString(0, 3, " Демо: вкл  ", playState == DEMO);
    else
      DisplayWriteString(0, 3, " Демо: выкл  ", playState == DEMO);
  }
}

void RgbLedFunc(int32_t id)
{
  static uint8_t funcType = 1;
  static T_RGB_LED rgbLeds[LEDS_NUM] = {0};
  if(id < 0)
  {
    switch(id)
    {
    case -1:
      funcType = 1;
      break;
    case -2:
      funcType = 2;
      break;
    case -3:
      funcType = 3;
      break;
    default:
      return;
    }
    id = CALL_SHOW_ID;
    DisplayClear();
  }
  
  if(id == BUT_X)
  {
    SoftTimerStop(SOFT_TIMER_DYNAMIC);
    memset(rgbLeds, 0, sizeof(rgbLeds));
    RgbLedWrite(rgbLeds, LEDS_NUM);
    MenuFromFunc();
  }
  else
  {
    switch(funcType)
    {
    case 1:
      {
        static uint32_t ledInd = 0, prevInd = 0;
        static T_RGB_LED ledColor;
        static uint32_t timeOut = 100, perCent = 5;
        static uint8_t flowCntr = 0, curItem = 0;
        if(id == CALL_SHOW_ID)
        {
          ledInd = 0;
          flowCntr = 0;
          ledColor.r = (rand() % 10) * perCent;
          ledColor.g = (rand() % 10) * perCent;
          ledColor.b = (rand() % 10) * perCent;
          rgbLeds[ledInd] = ledColor;
          RgbLedWrite(rgbLeds, LEDS_NUM);
          SoftTimerStart(SOFT_TIMER_DYNAMIC, timeOut, 0, CALL_TIMR_ID, NULL, RgbLedFunc);
        }
        else if(id == CALL_TIMR_ID)
        {
          uint32_t curTimeout = 20;
          flowCntr++;
          if(flowCntr == 4)
          {
            flowCntr = 0;
            rgbLeds[ledInd].b = ledColor.b;
            rgbLeds[ledInd].g = ledColor.g;
            rgbLeds[ledInd].r = ledColor.r;
            curTimeout = timeOut;
          }
          else if(flowCntr == 3)
          {
            memset(&rgbLeds[prevInd], 0, sizeof(*rgbLeds));
            rgbLeds[ledInd].b = ledColor.b / 2;
            rgbLeds[ledInd].g = ledColor.g / 2;
            rgbLeds[ledInd].r = ledColor.r / 2;
          }
          else if(flowCntr == 2)
          {
            rgbLeds[ledInd].b = ledColor.b / 4;
            rgbLeds[ledInd].g = ledColor.g / 4;
            rgbLeds[ledInd].r = ledColor.r / 4;
            prevInd = ledInd;
            if(++ledInd == LEDS_NUM)
            {
              ledInd = 0;
              ledColor.r = (rand() % 10) * perCent;
              ledColor.g = (rand() % 10) * perCent;
              ledColor.b = (rand() % 10) * perCent;
            }
            rgbLeds[ledInd].b = ledColor.b / 4;
            rgbLeds[ledInd].g = ledColor.g / 4;
            rgbLeds[ledInd].r = ledColor.r / 4;
          }
          else
          {
            rgbLeds[ledInd].b = ledColor.b / 2;
            rgbLeds[ledInd].g = ledColor.g / 2;
            rgbLeds[ledInd].r = ledColor.r / 2;
          }
          RgbLedWrite(rgbLeds, LEDS_NUM);
          SoftTimerStart(SOFT_TIMER_DYNAMIC, curTimeout, 0, CALL_TIMR_ID, NULL, RgbLedFunc);
        }
        else if(id == BUT_UP)
        {
          if(curItem == 0)
          {
            if(timeOut > 25)
              timeOut -= 5;
            else
              timeOut = 20;
          }
          else if(curItem == 1)
          {
            if(perCent < 9)
              perCent += 1;
            else
              perCent = 10;
          }
        }
        else if(id == BUT_DOWN)
        {
          if(curItem == 0)
          {
            if(timeOut < 145)
              timeOut += 5;
            else
              timeOut = 150;
          }
          else if(curItem == 1)
          {
            if(perCent > 2)
              perCent -= 1;
            else
              perCent = 1;
          }
        }
        else if(id == BUT_MENU || BUT_MENU1)
        {
          if(++curItem >= 2)
            curItem = 0;
        }
        char str[SYMB_PER_LINE + 1];
        sprintf(str, "Скорость:%3d", (150 - timeOut) / 5);
        DisplayWriteString(0, 1, str, curItem == 0);
        sprintf(str, "Яркость:%3d%%", perCent * 10);
        DisplayWriteString(0, 2, str, curItem == 1);
      }
      break;
    case 2:
      {
        static uint32_t ledInd = 0, perCent = 3;
        static bool ledActiveFlag = false;
        if(id == CALL_SHOW_ID || id == CALL_TIMR_ID)
        {
          uint32_t curTimeOut;
          if(ledActiveFlag)
          {
            memset(&rgbLeds[ledInd], 0, sizeof(*rgbLeds));
            curTimeOut = 100 + rand() % 500;
          }
          else
          {
            ledInd = rand() % LEDS_NUM;
            rgbLeds[ledInd].r = (rand() % 10) * perCent;
            rgbLeds[ledInd].g = (rand() % 10) * perCent;
            rgbLeds[ledInd].b = (rand() % 10) * perCent;
            curTimeOut = 50;
          }
          ledActiveFlag = !ledActiveFlag;
          
          RgbLedWrite(rgbLeds, LEDS_NUM);
          SoftTimerStart(SOFT_TIMER_DYNAMIC, curTimeOut, 0, CALL_TIMR_ID, NULL, RgbLedFunc);
        }
        else if(id == BUT_DOWN)
        {
          if(perCent > 2)
            perCent -= 1;
          else
            perCent = 1;
        }
        else if(id == BUT_UP)
        {
          if(perCent < 9)
            perCent += 1;
          else
            perCent = 10;
        }
        char str[SYMB_PER_LINE + 1];
        sprintf(str, "Яркость:%3d%%", perCent * 10);
        DisplayWriteString(0, 1, str, false);
      }
      break;
    case 3:
      {
        static uint8_t brightness = 30 * 255 / 100;
        static uint8_t r = 0, g = 0, b = 0, delta = 1;
        static int8_t rDir = 1, gDir = 0, bDir = 0;
        if(id == CALL_SHOW_ID || id == CALL_TIMR_ID)
        {
          if(rDir > 0)
          {
             if(r < brightness - delta)
             {
               r += delta;
             }
             else
             {
               r = brightness;
               rDir = 0;
               bDir = -1;
             }
          }
          else if(rDir < 0)
          {
            if(r > delta)
            {
              r -= delta;
            }
            else
            {
              r = 0;
              rDir = 0;
              bDir = 1;
            }
          }
          if(gDir > 0)
          {
             if(g < brightness - delta)
             {
               g += delta;
             }
             else
             {
               g = brightness;
               gDir = 0;
               rDir = -1;
             }
          }
          else if(gDir < 0)
          {
            if(g > delta)
            {
              g -= delta;
            }
            else
            {
              g = 0;
              gDir = 0;
              rDir = 1;
            }
          }
          if(bDir > 0)
          {
             if(b < brightness - delta)
             {
               b += delta;
             }
             else
             {
               b = brightness;
               bDir = 0;
               gDir = -1;
             }
          }
          else if(bDir < 0)
          {
            if(b > delta)
            {
              b -= delta;
            }
            else
            {
              b = 0;
              bDir = 0;
              gDir = 1;
            }
          }
          
          uint32_t i;
          for(i = 0; i < LEDS_NUM; i++)
          {
            rgbLeds[i].r = r;
            rgbLeds[i].g = g;
            rgbLeds[i].b = b;
          }
          RgbLedWrite(rgbLeds, LEDS_NUM);
          SoftTimerStart(SOFT_TIMER_DYNAMIC, 2000 / brightness, 0, CALL_TIMR_ID, NULL, RgbLedFunc);
        }
        else if(id == BUT_DOWN)
        {
          if(brightness > 10)
            brightness -= 5;
          else
            brightness = 10;
        }
        else if(id == BUT_UP)
        {
          if(brightness < 0xFF)
            brightness += 5;
          else
            brightness = 0xFF;
        }
        char str[SYMB_PER_LINE + 1];
        sprintf(str, "Яркость:%3d%%", brightness * 100 / 0xFF);
        DisplayWriteString(0, 1, str, false);
      }
      break;
    default:
      break;
    }
  }
}