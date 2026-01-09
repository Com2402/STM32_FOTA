#ifndef __UART_H
#define __UART_H

#include "main.h"

#define RX_BUFFER_SIZE 256

void UART_Init(void);
uint8_t UART_Receive(void);
void UART_Transmit(uint8_t data);
uint8_t UART_Available(void);
void UART_IRQHandler(void);

#endif