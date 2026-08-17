/**
 * @file    serial.c
 * @brief   UART 串口通信模块 (115200, 8N1)
 *
 *         基于 DL_UART_Main 驱动, 实现 printf 重定向及格式化输出
 */

#include "User.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UART_INST UART_0_INST


/* ========================================================================
 *  基础发送 / 接收
 * ======================================================================== */

/**
 * @brief  发送单个字节 (阻塞式)
 * @param  Byte  要发送的字节
 */
void send_Byte(uint8_t Byte)
{
    DL_UART_Main_transmitDataBlocking(UART_INST, Byte);
    /* 等待 TX FIFO 清空 + 移位寄存器发送完成, 确保字节完全发出 */
    while ((UART_0_INST->STAT & 0x40U) == 0U); /* BUSY */
}

/**
 * @brief  发送字符串 (以 '\0' 结尾)
 * @param  str  字符串指针
 */
void send_String(char *str)
{
    while (*str != '\0') {
        send_Byte(*str);
        str++;
    }
}

/**
 * @brief  接收单个字节 (阻塞式)
 * @return 接收到的字节
 */
uint8_t Accept_bytes(void)
{
    return DL_UART_Main_receiveDataBlocking(UART_INST);
}


/* ========================================================================
 *  printf 重定向 (使 printf 输出到 UART)
 * ======================================================================== */

/**
 * @brief  fputc 重定向 — 使标准库 printf 输出到 UART
 * @param  _c   要输出的字符
 * @param  _fp  文件指针 (未使用)
 * @return 输出的字符
 */
int fputc(int _c, FILE *_fp)
{
    DL_UART_Main_transmitDataBlocking(UART_INST, _c);
    return _c;
}


/* ========================================================================
 *  非阻塞接收
 * ======================================================================== */

/**
 * @brief  非阻塞接收单个字节（轮询 RX FIFO）
 * @param  byte  接收数据存放指针
 * @return true=有数据并已读出, false=无数据
 */
bool UART_ReceiveNonBlocking(uint8_t *byte)
{
    /* STAT[0] RXFE: 0=FIFO非空(有数据), 1=FIFO空 */
    if ((UART_0_INST->STAT & 0x01U) == 0U) {
        *byte = (uint8_t)(UART_0_INST->RXDATA);
        return true;
    }
    return false;
}

/**
 * @brief  格式化 UART 打印 (内部缓冲区 128 字节, 不依赖 fputc)
 *
 *         vsnprintf → TxBuffer[] → 逐字节阻塞发送
 *
 * @param  format  格式化字符串
 * @param  ...     可变参数
 */
void UART_printf(const char *format, ...)
{
    uint32_t length;
    va_list  args;
    uint32_t i;
    char     TxBuffer[128] = {0};

    va_start(args, format);
    length = vsnprintf((char *)TxBuffer, sizeof(TxBuffer), (char *)format, args);
    va_end(args);

    for (i = 0; i < length; i++) {
        DL_UART_Main_transmitDataBlocking(UART_INST, TxBuffer[i]);
    }
}


