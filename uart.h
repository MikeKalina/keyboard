#ifndef UART_H_
#define UART_H_

#include "stm32l1xx.h"
#include <stdint.h>
#include <stdbool.h>

#define UART1_ENABLE
#define UART2_ENABLE_
#define UART1_DMA_TX
#define UART2_DMA_TX

void UartInit();

#ifdef UART1_ENABLE
void UART1_Send(uint8_t *data, uint16_t len);
#endif
#ifdef UART2_ENABLE
void UART2_Send(uint8_t *data, uint16_t len);
#endif

#endif /* UART_H_ */