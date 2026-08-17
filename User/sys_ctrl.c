/******************************************************************************
 *  系统控制器
 *  统一管理：硬件初始化、NRF握手、主循环调度
 ******************************************************************************/

#include "sys_ctrl.h"
#include "../ti_msp_dl_config.h"
#include "Delay.h"
#include "NRF24L01.h"
#include "nrf_system.h"
#include <stdio.h>

/* ============================================================
 *  初始化
 * ============================================================ */
void sys_ctrl_init(void)
{
    // OLED 初始化由用户在 empty.c 中自行调用
}

/* ============================================================
 *  握手等待（永不返回，成功后跳入用户任务）
 * ============================================================ */
#if defined(SYS_MODE_HANDSHAKE)

static void handshake_car(sys_task_cb_t task)
{
    nrf_system_init();
    printf("Waiting icmok...\r\n");

    while (1) {
        nrf_system_receive();
        if (nrf_icm_ready) {
            nrf_icm_ready = 0;
            printf("Got icmok!\r\n");
            break;
        }
        delay_ms(1);
    }
}

static void handshake_remote(sys_task_cb_t task)
{
    nrf_system_init();

    uint8_t *tx = nrf_system_tx_packet();
    tx[0] = 'i'; tx[1] = 'c'; tx[2] = 'm'; tx[3] = 'o'; tx[4] = 'k';

    while (1) {
        if (nrf_system_send() == NRF_SEND_OK) {
            break;
        }
        delay_ms(200);
    }
}

#endif /* SYS_MODE_HANDSHAKE */

/* ============================================================
 *  主循环
 * ============================================================ */
void sys_ctrl_run(sys_task_cb_t task)
{
#if defined(SYS_MODE_HANDSHAKE) && defined(SYS_ROLE_REMOTE)
    handshake_remote(task);
    /* 遥控端握手后静默 */
    while (1) { delay_ms(1000); }
#elif defined(SYS_MODE_HANDSHAKE) && defined(SYS_ROLE_CAR)
    handshake_car(task);
    printf("SYS: enter main loop\r\n");
    while (1) {
        uint16_t dt = task();
        if (dt > 0) delay_ms(dt);
    }
#else
    printf("SYS: standalone mode\r\n");
    printf("SYS: enter main loop\r\n");
    while (1) {
        uint16_t dt = task();
        if (dt > 0) delay_ms(dt);
    }
#endif
}
