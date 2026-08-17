/**
 * @file    empty.c
 * @brief   UART 通信验证 — 利用现有 g_stop_notify_cnt, ISR 每2ms发'0', 共300ms(150个)
 */

#include "ti_msp_dl_config.h"
#include "User.h"

int main(void)
{
    SYSCFG_DL_init();
    delay_ms(100);   /* 等外设稳定 */

    /* 触发 ISR 内非阻塞发 '0': 150 × 2ms = 300ms */
    g_stop_notify_cnt = 150;

    send_String("--- M0 start, sending '0' every 2ms x150 ---\r\n");
send_String("--- cxlcxl");
	send_String("--- cxjcxj");
    /* ── 等待 32 回复 ── */
    while (1) {
        uint8_t rx;
        if (UART_ReceiveNonBlocking(&rx)) {
            if (rx == '0') {
                g_stop_notify_cnt = 0;   /* 32已收到, 停止发送 */
                send_String("M0 RX: got '0' from 32! STOP sending.\r\n");
            } else {
                UART_printf("M0 RX: byte='%c' (0x%02X)\r\n", rx, rx);
            }
        }
        delay_ms(10);
    }
}
