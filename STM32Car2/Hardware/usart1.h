#ifndef __USART1_H
#define __USART1_H

void Usart1_Init(void);
void USART1_SendByte(uint8_t Byte);
void USART1_SendString(char *String);
int fputc(int ch, FILE *f);
#endif



