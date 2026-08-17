#ifndef __PID_H
#define __PID_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 *  速度环 PID (增量式, M1 + M2 独立控制)
 * ======================================================================== */

void SpeedPID_Init(void);
void SpeedPID_SetTarget(int32_t spd_m1, int32_t spd_m2);   /* 编码器脉冲/10ms */
void SpeedPID_Task1(void);    /**< 模式1: 高速50 */
void SpeedPID_Task2(void);    /**< 模式2: 低速30 */
void SpeedPID_Task3(void);    /**< 模式3: 低速定时5s自动停 */
void SpeedPID_Start1(void);   /**< KEY1 启动高速模式 */
void SpeedPID_Start2(void);   /**< KEY2 启动低速模式 */
void SpeedPID_Start3(void);   /**< 长按KEY2 启动定时模式 */
void SpeedPID_Stop(void);         /**< 轻量停车(ISR安全), 无UART/蜂鸣 */
void SpeedPID_Stop_Notify(void); /**< 主循环用: 发'0'+停车+蜂鸣 */
bool SpeedPID_IsRunning(void);
uint8_t SpeedPID_GetMode(void); /**< 0=停,1=高速,2=低速,3=定时 */

/** ISR 内全黑停车后置位, 主循环检测后响蜂鸣并清零 */
extern volatile bool g_full_black_stop;

/** ISR 内非阻塞发送 '0' 计数器, 主循环按键停车时也可置位 */
extern volatile uint16_t g_stop_notify_cnt;

#ifdef __cplusplus
}
#endif

#endif
