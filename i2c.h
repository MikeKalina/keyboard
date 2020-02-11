#ifndef I2C_H
#define I2C_H

#include "stm32l1xx.h"
#include <stdint.h>
#include <stdbool.h>

void I2cInit();
void I2C1_WriteByte(uint8_t device_address, uint8_t value);
void I2C1_WriteTwoByte(uint8_t device_address, uint8_t value_a, uint8_t value_b);
uint8_t I2C1_ReadByte(uint8_t device_address);
void I2C1_ReadBytes(uint8_t device_address, uint8_t *data, uint8_t len);

#endif