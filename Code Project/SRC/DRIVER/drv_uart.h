#ifndef _DRV_UART_H
#define _DRV_UART_H

#include "board.h"

#include "usart.h"

void Uart_Init();
void Uart_SendData(uint8_t deviceNum, uint8_t *DataToSend,uint8_t length);

#endif
