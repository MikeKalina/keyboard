#ifndef SPI_H_
#define SPI_H_

#include "stm32l1xx.h"
#include <stdint.h>
#include <stdbool.h>

#define SPI_1_ENABLE
#define SPI1_DMA_TX_
#define ENABLE_SOFT_CS

#define SPI_2_ENABLE
#define SPI2_DMA_TX

void SpiInit();

#ifdef SPI_1_ENABLE
void SPI1_Send(uint8_t *data, uint16_t size);
#endif

#ifdef SPI_2_ENABLE
void SPI2_Send(uint8_t *data, uint16_t size);
#endif

#endif