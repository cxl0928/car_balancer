#ifndef SYS_CTRL_H
#define SYS_CTRL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 *  系统控制器 — 所有配置集中在这里
 * ============================================================ */

// ── 运行模式（二选一） ──
#define SYS_MODE_STANDALONE     // 单机模式：跳过 NRF，直接进入主循环
//#define SYS_MODE_HANDSHAKE    // 握手模式：通过 NRF 握手成功后才启动

// ── 握手角色（仅在 SYS_MODE_HANDSHAKE 时有效，二选一） ──
//#define SYS_ROLE_CAR          // 接收端
//#define SYS_ROLE_REMOTE       // 发送端

// ── 用户任务回调类型 ──
// 由用户实现，每周期调用一次，返回下次调用的间隔(ms)
// 典型用法：读取传感器 → 控制电机 → 显示 → return 50
typedef uint16_t (*sys_task_cb_t)(void);

/* ============================================================
 *  API
 * ============================================================ */

/* 初始化所有硬件模块并显示启动信息 */
void sys_ctrl_init(void);

/*
 * 启动系统主循环（永不返回）
 * @param task  用户任务回调，每周期调用
 * @param idle  空闲时调用的函数（当不需要 NRF 时直接跳入主循环，传 NULL 则用默认空循环）
 */
void sys_ctrl_run(sys_task_cb_t task);

#ifdef __cplusplus
}
#endif

#endif /* SYS_CTRL_H */
