#include "uart.h"

#ifdef UART1_ENABLE
void UART1_Init();

#ifdef UART1_DMA_TX
void Uart1DmaSend(uint8_t *data, uint16_t len);
#endif /* UART1_DMA_TX */

volatile static uint8_t rcvDataUart1[256];
volatile static uint8_t iRxUart1 = 0;

#endif /* UART1_ENABLE */

#ifdef UART2_ENABLE
void UART2_Init();

#ifdef UART2_DMA_TX
void Uart2DmaSend(uint8_t *data, uint16_t len);
#endif /* UART2_DMA_TX */

volatile static uint8_t rcvDataUart2[256];
volatile static uint8_t iRxUart2 = 0;

#endif /* UART2_ENABLE */

void UartInit()
{
#ifdef UART1_ENABLE
  UART1_Init();
#endif
  
#ifdef UART2_ENABLE
  UART2_Init();
#endif
}

#ifdef UART1_ENABLE
void UART1_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);// uart1 TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);// uart1 RX
  
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_Init(USART1, &USART_InitStructure);
  
  NVIC_EnableIRQ(USART1_IRQn);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  
#ifdef UART1_DMA_TX
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
  
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
#endif
  
  USART_Cmd(USART1, ENABLE);
}

void UART1_Send(uint8_t *data, uint16_t len)
{
#ifdef UART1_DMA_TX
  Uart1DmaSend(data, len);
#else
  while(len--)
  {
    /* while TXE bit is 0 - Data is not transferred to the shift register */
    while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
    USART_SendData(USART1, *data++);
  }
  /* while TC bit is 0 - Transmission is not complete */
  while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
#endif
}

void USART1_IRQHandler()
{
  if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    rcvDataUart1[iRxUart1++] = USART_ReceiveData(USART1) & 0xff;
  }
}

#ifdef UART1_DMA_TX
  void Uart1DmaSend(uint8_t *data, uint16_t len)
  {
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
    dmaInit.DMA_MemoryBaseAddr = (uint32_t)data;
    dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
    dmaInit.DMA_BufferSize = len;
    dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaInit.DMA_Mode = DMA_Mode_Normal;
    dmaInit.DMA_Priority = DMA_Priority_Medium;
    dmaInit.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel4, &dmaInit);
    
    DMA_Cmd(DMA1_Channel4, ENABLE);
  }
  
  void DMA1_Channel4_IRQHandler(void)
  {
    if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
    {
      DMA_ClearITPendingBit(DMA1_IT_TC4);
      DMA_Cmd(DMA1_Channel4, DISABLE);
    }
  }
#endif /* UART1_DMA_TX */
#endif /* UART1_ENABLE */

#ifdef UART2_ENABLE
void UART2_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART1);// uart2 TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART1);// uart2 RX
  
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_Init(USART2, &USART_InitStructure);
  
  NVIC_EnableIRQ(USART2_IRQn);
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  
#ifdef UART2_DMA_TX
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
  
  NVIC_EnableIRQ(DMA1_Channel7_IRQn);
  DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
#endif
  
  USART_Cmd(USART2, ENABLE);
}

void UART2_Send(uint8_t *data, uint16_t len)
{
#ifdef UART2_DMA_TX
  Uart2DmaSend(data, len);
#else
  while(len--)
  {
    /* while TXE bit is 0 - Data is not transferred to the shift register */
    while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
    USART_SendData(USART2, *data++);
  }
  /* while TC bit is 0 - Transmission is not complete */
  while(!USART_GetFlagStatus(USART2, USART_FLAG_TC));
#endif
}

void USART2_IRQHandler()
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
  {
    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    rcvDataUart2[iRxUart2++] = USART_ReceiveData(USART2);
  }
}

#ifdef UART2_DMA_TX
  void Uart2DmaSend(uint8_t *data, uint16_t len)
  {
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);
    dmaInit.DMA_MemoryBaseAddr = (uint32_t)data;
    dmaInit.DMA_DIR = DMA_DIR_PeripheralDST;
    dmaInit.DMA_BufferSize = len;
    dmaInit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaInit.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaInit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaInit.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaInit.DMA_Mode = DMA_Mode_Normal;
    dmaInit.DMA_Priority = DMA_Priority_Medium;
    dmaInit.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &dmaInit);
    
    DMA_Cmd(DMA1_Channel7, ENABLE);
  }
  
  void DMA1_Channel7_IRQHandler(void)
  {
    if(DMA_GetITStatus(DMA1_IT_TC7) == SET)
    {
      DMA_ClearITPendingBit(DMA1_IT_TC7);
      DMA_Cmd(DMA1_Channel7, DISABLE);
    }
  }
#endif /* UART2_DMA_TX */
#endif /* UART2_ENABLE */
