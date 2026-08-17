/**
 * @file    peripheral.h
 * @brief   蜂鸣器 + PC817 + LED1/LED2 + key1/key2 简易控制接口
 */

#ifndef __PERIPHERAL_H__
#define __PERIPHERAL_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== 蜂鸣器 (PWM_3, PA31) ==================== */

/**
 * @brief  设置蜂鸣器占空比
 * @param  duty  0~100，0=静音，100=最大声
 */
void Buzzer(uint8_t duty);

/* ==================== PC817 光耦 (PB12) ==================== */

void PC817(bool on);   /* true=导通, false=关断 */

/* ==================== LED1 (PA8) / LED2 (PA28) ==================== */

void LED1(bool on);    /* true=亮, false=灭 */
void LED2(bool on);    /* true=亮, false=灭 */

/* ==================== 按键 key1 (PB6) / key2 (PB23) ==================== */

bool KEY1(void);       /* true=按下（低电平）, false=释放 */
bool KEY2(void);       /* true=按下（低电平）, false=释放 */

#ifdef __cplusplus
}
#endif

#endif /* __PERIPHERAL_H__ */
