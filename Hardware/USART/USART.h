#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define MAX_RX_DATA_LENGTH 128 

void USART1_Init(void);
void USART_SendByte(uint8_t Byte);
void USART_SendString(char *String);
#endif
