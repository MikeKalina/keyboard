#ifndef MPU_6050_H
#define MPU_6050_H

#include <stdint.h>
#include <stdbool.h>

#define MPU_ADDRESS     0x68/* address of mpu for AD0 at low */
#define MPU_ADDRESS1_    0x69/* address of mpu for AD0 at high */

#define MPU_ID          0x68
#define MPU_ID_REG      0x75
#define MPU_ACC_REG     0x3B
#define MPU_TMP_REG     0x41
#define MPU_GYR_REG     0x43

#define MPU_CALIB_SAMPLES_NUM        1000

typedef struct
{
  int16_t x;
  int16_t y;
  int16_t z;
} T_VECTOR_16;

typedef struct
{
  int32_t x;
  int32_t y;
  int32_t z;
} T_VECTOR_32;

typedef struct
{
  float x;
  float y;
  float z;
} T_VECTOR_FLOAT;

typedef struct
{
  T_VECTOR_16 acc;
  T_VECTOR_16 gyr;
  T_VECTOR_FLOAT accScale;
  T_VECTOR_16 gyrOffset;
  T_VECTOR_32 gyrSumOffset;
} T_MPU_STRUCT;

bool MpuInit();
bool MpuItemInit(uint8_t addr);

void MpuReadID(uint8_t addr, uint8_t *id);
void MpuReadAcc(uint8_t addr);
void MpuReadGyr(uint8_t addr);
void MpuReadTemp(uint8_t addr, float *t);

void MpuGetAcc(T_VECTOR_16 *acc);
void MpuGetGyr(T_VECTOR_16 *gyr);

void MpuWriteGyrOffset(uint8_t addr);
void MpuSetGyrOffset(uint32_t offsetNum);

#endif