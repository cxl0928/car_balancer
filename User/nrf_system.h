#ifndef NRF_SYSTEM_H
#define NRF_SYSTEM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 *  NRF24L01 通信封装层
 *  - 屏蔽底层 NRF24L01 驱动细节
 *  - 提供统一的 init / send / receive 接口
 *  - 角色/握手由 sys_ctrl.h 统一管理
 * ============================================================ */

#define NRF_PACKET_SIZE     5       // 数据包字节数（与底层 NRF24L01_TX_PACKET_WIDTH 一致）

/* 发送/接收结果 */
#define NRF_SEND_OK         1       // 发送成功（收到 ACK）
#define NRF_SEND_NO_ACK     2       // 发送失败（无 ACK，重传耗尽）
#define NRF_SEND_ERR        3       // 发送超时

#define NRF_RECV_OK         1       // 收到新数据
#define NRF_RECV_NONE       0       // 无数据
#define NRF_RECV_ERR        3       // 模块异常（已自动重建）

/* 标志位：收到 "icmok" 后置 1，由 main 读取并清零 */
extern volatile uint8_t nrf_icm_ready;

/* 获取全局数据包指针 */
uint8_t *nrf_system_tx_packet(void);
uint8_t *nrf_system_rx_packet(void);

/* 初始化 */
void nrf_system_init(void);

/* 发送（填充 tx_packet 后调用） */
uint8_t nrf_system_send(void);

/* 接收（非阻塞轮询，返回 NRF_RECV_OK 时 rx_packet[] 有效；收到 "icmok" 自动置位 nrf_icm_ready） */
uint8_t nrf_system_receive(void);

#ifdef __cplusplus
}
#endif

#endif /* NRF_SYSTEM_H */
