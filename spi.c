#include "spi.h"

#ifdef SPI_1_ENABLE
void SPI1_Init();

#ifdef SPI1_DMA_TX
void Spi1DmaSend(uint8_t *data, uint16_t len);
#endif /* SPI1_DMA_TX */
#endif

#ifdef SPI_2_ENABLE
void SPI2_Init();

#ifdef SPI2_DMA_TX
void Spi2DmaSend(uint8_t *data, uint16_t len);
#endif /* SPI1_DMA_TX */
#endif


void SpiInit()
{
#ifdef SPI_1_ENABLE
  SPI1_Init();
#endif
#ifdef SPI_2_ENABLE
  SPI2_Init();
#endif
}

#ifdef SPI_1_ENABLE
void SPI1_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = 
    GPIO_Pin_5
//    | GPIO_Pin_6
    | GPIO_Pin_7
#ifndef ENABLE_SOFT_CS
    | GPIO_Pin_4
#endif
    ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
#ifndef ENABLE_SOFT_CS
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI1);// NSS
#endif
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);// SCK
//  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);// MISO
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);// MOSI
  
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = 
#ifdef ENABLE_SOFT_CS
    SPI_NSS_Soft
#else
    SPI_NSS_Hard
#endif
      ;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//clock 16MHz
  SPI_Init(SPI1, &SPI_InitStructure);
  
#ifdef SPI1_DMA_TX
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  SPI_DMACmd(SPI1, SPI_DMAReq_Tx, ENABLE);
  
  NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
#endif
  
  /* Enable the SPI peripheral */
  SPI_Cmd(SPI1, ENABLE);
#ifndef ENABLE_SOFT_CS
  /* Enable NSS output for master mode */
  SPI_SSOutputCmd(SPI1, ENABLE);
#endif
}

void SPI1_Send(uint8_t *data, uint16_t size)
{
#ifdef SPI1_DMA_TX
  Spi1DmaSend(data, size);
#else
  while(size--)
  {
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, *data++);
  }
  while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
#endif
}

#ifdef SPI1_DMA_TX
  void Spi1DmaSend(uint8_t *data, uint16_t len)
  {
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)(&SPI1->DR);
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
    DMA_Init(DMA1_Channel3, &dmaInit);
    
    DMA_Cmd(DMA1_Channel3, ENABLE);
  }
  
  void DMA1_Channel3_IRQHandler(void)
  {
    if(DMA_GetITStatus(DMA1_IT_TC3) == SET)
    {
      DMA_ClearITPendingBit(DMA1_IT_TC3);
      DMA_Cmd(DMA1_Channel3, DISABLE);
    }
  }
#endif /* SPI1_DMA_TX */
#endif

#ifdef SPI_2_ENABLE
void SPI2_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = 
//    GPIO_Pin_13
//    | GPIO_Pin_14
//    | 
    GPIO_Pin_15
    ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);// SCK
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);// MISO
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);// MOSI
  
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_Init(SPI2, &SPI_InitStructure);
  
#ifdef SPI2_DMA_TX
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  SPI_DMACmd(SPI2, SPI_DMAReq_Tx, ENABLE);
  
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
#endif
  
  /* Enable the SPI peripheral */
  SPI_Cmd(SPI2, ENABLE);
}

void SPI2_Send(uint8_t *data, uint16_t size)
{
#ifdef SPI2_DMA_TX
  Spi2DmaSend(data, size);
#else
  while(size--)
  {
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI2, *data++);
  }
  while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
#endif
}

#ifdef SPI2_DMA_TX
  void Spi2DmaSend(uint8_t *data, uint16_t len)
  {
    DMA_InitTypeDef dmaInit;
    dmaInit.DMA_PeripheralBaseAddr = (uint32_t)(&SPI2->DR);
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
    DMA_Init(DMA1_Channel5, &dmaInit);
    
    DMA_Cmd(DMA1_Channel5, ENABLE);
  }
  
  void DMA1_Channel5_IRQHandler(void)
  {
    if(DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
      DMA_ClearITPendingBit(DMA1_IT_TC5);
      DMA_Cmd(DMA1_Channel5, DISABLE);
    }
  }
#endif /* UART2_DMA_TX */
#endif