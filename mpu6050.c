#include "mpu6050.h"
#include "i2c.h"
#include "string.h"

T_MPU_STRUCT mpu;

bool MpuInit()
{
  if(!MpuItemInit(MPU_ADDRESS))
    return false;
  memset(&mpu, 0, sizeof(mpu));
  return true;
}

bool MpuItemInit(uint8_t addr)
{
  // reset
  I2C1_WriteTwoByte(addr, 0x6B, 0x80);
  uint16_t cntr = 0xffff;
  while(cntr--);// waiting
  uint8_t mpuId;
  MpuReadID(addr, &mpuId);
  if(mpuId != MPU_ID)
    return false;
  I2C1_WriteTwoByte(addr, 0x6B, 0x00);
  I2C1_WriteTwoByte(addr, 0x1B, 0x08);
  I2C1_WriteTwoByte(addr, 0x1C, 0x08);
  cntr = 0xffff;
  while(cntr--);// waiting
  return true;
}

void MpuReadID(uint8_t addr, uint8_t *id)
{
  I2C1_WriteByte(addr, MPU_ID_REG);// указатель на регистр
  I2C1_ReadBytes(addr, id, 1);
}

void MpuReadAcc(uint8_t addr)
{
  uint8_t mas[6];
  
  I2C1_WriteByte(addr, MPU_ACC_REG);// указатель на начало регистров
  I2C1_ReadBytes(addr, mas, 6);
  
  mpu.acc.x = ((int16_t)mas[0]) << 8 | (int16_t)mas[1];
  mpu.acc.y = ((int16_t)mas[2]) << 8 | (int16_t)mas[3];
  mpu.acc.z = ((int16_t)mas[4]) << 8 | (int16_t)mas[5];
}

void MpuReadGyr(uint8_t addr)
{
  uint8_t mas[6];
  
  I2C1_WriteByte(addr, MPU_GYR_REG);// указатель на начало регистров
  I2C1_ReadBytes(addr, mas, 6);
  
  mpu.gyr.x = ((int16_t)mas[0]) << 8 | (int16_t)mas[1];
  mpu.gyr.y = ((int16_t)mas[2]) << 8 | (int16_t)mas[3];
  mpu.gyr.z = ((int16_t)mas[4]) << 8 | (int16_t)mas[5];
}

void MpuReadTemp(uint8_t addr, float *t)
{
  uint8_t mas[2];
  
  I2C1_WriteByte(addr, MPU_TMP_REG);// указатель на начало регистров
  I2C1_ReadBytes(addr, mas, 2);
  
  int16_t temp = ((int16_t)mas[0]) << 8 | (int16_t)mas[1];
  *t = (float)temp / 340 + 36.53;
}

void MpuGetAcc(T_VECTOR_16 *acc)
{
  acc->x = mpu.acc.x;
  acc->y = mpu.acc.y;
  acc->z = mpu.acc.z;
}

void MpuGetGyr(T_VECTOR_16 *gyr)
{
  gyr->x = mpu.gyr.x - mpu.gyrOffset.x;
  gyr->y = mpu.gyr.y - mpu.gyrOffset.y;
  gyr->z = mpu.gyr.z - mpu.gyrOffset.z;
}

void MpuWriteGyrOffset(uint8_t addr)
{
  MpuReadGyr(addr);
  mpu.gyrSumOffset.x += mpu.gyr.x;
  mpu.gyrSumOffset.y += mpu.gyr.y;
  mpu.gyrSumOffset.z += mpu.gyr.z;
}

void MpuSetGyrOffset(uint32_t offsetNum)
{
  mpu.gyrOffset.x = mpu.gyrSumOffset.x / (int32_t)offsetNum;
  mpu.gyrOffset.y = mpu.gyrSumOffset.y / (int32_t)offsetNum;
  mpu.gyrOffset.z = mpu.gyrSumOffset.z / (int32_t)offsetNum;
}