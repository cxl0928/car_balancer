#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

void send_Byte(uint8_t Byte);
void send_String(char *str);
uint8_t Accept_bytes(void);
int fputc(int ch, FILE *f);
void UART_printf(const char *format, ...);

/**
 * @brief  非阻塞接收一个字节
 * @param  byte  接收数据存放指针
 * @return true=读到数据, false=无数据
 */
bool UART_ReceiveNonBlocking(uint8_t *byte);

#endif
