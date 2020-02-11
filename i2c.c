#include "i2c.h"
#include "gpio.h"

void I2C1_Init(void);

void I2cInit()
{
  I2C1_Init();
}

//------------------------------------------------------------------------------
void I2C1_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure; 
  I2C_InitTypeDef I2C_InitStructure;	  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE); 	
  //***************************************************************************
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);// SCL
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);// SDA
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;	
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;
  I2C_Init(I2C1, &I2C_InitStructure);
  
  I2C_Cmd(I2C1, ENABLE);
}

void I2C1_WriteByte(uint8_t device_address, uint8_t value)
{
  /* формируем сигнал СТАРТ */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* ждем окончания передачи сигнала СТАРТ */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  
  /* передаем адрес ведомого устройства */
  I2C_Send7bitAddress(I2C1, device_address << 1, I2C_Direction_Transmitter);
  /* ждем окончания передачи адреса */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  I2C_ReadRegister(I2C1, I2C_Register_SR2);
  
  /* передаем значение */
  I2C_SendData(I2C1, value);
  /* ожидаем окончания передачи */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_BTF));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  
  /* формируем сигнал СТОП */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C1_WriteTwoByte(uint8_t device_address, uint8_t value_a, uint8_t value_b)
{
  /* формируем сигнал СТАРТ */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* ждем окончания передачи сигнала СТАРТ */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);

  /* передаем адрес ведомого устройства */
  I2C_Send7bitAddress(I2C1, device_address << 1, I2C_Direction_Transmitter);
  /* ждем окончания передачи адреса */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  I2C_ReadRegister(I2C1, I2C_Register_SR2);
  
  /* передаем первое значение */
  I2C_SendData(I2C1, value_a);
  /* ожидаем окончания передачи */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_BTF));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  
  /* передаем второе значение */
  I2C_SendData(I2C1, value_b);
  /* ожидаем окончания передачи */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_BTF));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  
  /* формируем сигнал СТОП */
  I2C_GenerateSTOP(I2C1, ENABLE);
}

void I2C1_ReadBytes(uint8_t device_address, uint8_t *data, uint8_t len)
{
  /* формируем сигнал СТАРТ */
  I2C_GenerateSTART(I2C1, ENABLE);
  /* ждем окончания передачи сигнала СТАРТ */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_SB));
  I2C_ReadRegister(I2C1, I2C_Register_SR1);
  
  /* передаем адрес ведомого устройства */
  I2C_Send7bitAddress(I2C1, device_address << 1, I2C_Direction_Receiver);
  /* ждем окончания передачи адреса */
  while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_ADDR));
  
  /* получаем данные от ведомого устройства */
  uint8_t i;
  for(i = 0; i < len; i++)
  {
    if(i < len - 1)
      I2C_AcknowledgeConfig(I2C1, ENABLE);
    else
      I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_ReadRegister(I2C1, I2C_Register_SR1);
    I2C_ReadRegister(I2C1, I2C_Register_SR2);
    
    /* ждем окончания приема данных */
    while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & I2C_SR1_RXNE));
    data[i] = I2C_ReceiveData(I2C1);
  }							
  
  /* формируем сигнал СТОП */
  I2C_GenerateSTOP(I2C1, ENABLE);
}
