#ifndef __USART1_H
#define __USART1_H
#include <stdio.h>
void Usart1_Init(void);
void USART1_SendByte(uint8_t Byte);
void PrintChar(char *s);
int fputc(int ch, FILE *f);
#endif



