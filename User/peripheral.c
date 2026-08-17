/**
 * @file    peripheral.c
 * @brief   蜂鸣器 + PC817 + LED1/LED2 + key1/key2 简易控制实现
 *
 * 硬件：
 *   蜂鸣器 : PWM_3 (TIMG7, PA31), 默认 2kHz
 *   PC817 : PB12
 *   LED1  : PA8
 *   LED2  : PA28
 *   key1  : PB6  (低电平按下)
 *   key2  : PB23 (低电平按下)
 */

#include "peripheral.h"
#include "../ti_msp_dl_config.h"

/* ==================== 蜂鸣器 (PWM_3) ==================== */

#define BUZZER_FREQ_HZ   2700        /* 默认蜂鸣器频率 2kHz */
#define BUZZER_CLK_HZ    32000000    /* PWM_3 时钟 32MHz */

void Buzzer(uint8_t duty)
{
    static bool inited = false;

    if (!inited) {
        /* 首次调用：设置 2kHz 固定频率 */
        uint32_t period = BUZZER_CLK_HZ / BUZZER_FREQ_HZ - 1;
        DL_TimerG_setLoadValue(PWM_3_INST, period);
        inited = true;
    }

    if (duty > 100) duty = 100;

    if (duty == 0) {
        DL_TimerG_setCaptureCompareValue(PWM_3_INST, 0, DL_TIMER_CC_1_INDEX);
    } else {
        uint32_t period = DL_TimerG_getLoadValue(PWM_3_INST);
        uint32_t cc_val = (period + 1) * duty / 100;
        DL_TimerG_setCaptureCompareValue(PWM_3_INST, cc_val, DL_TIMER_CC_1_INDEX);
    }
}

/* ==================== PC817 (PB12) ==================== */

void PC817(bool on)
{
    if (on)
        DL_GPIO_setPins(PC817_PORT, PC817_PIN_8_PIN);
    else
        DL_GPIO_clearPins(PC817_PORT, PC817_PIN_8_PIN);
}

/* ==================== LED1 (PA8) ==================== */

void LED1(bool on)
{
    if (on)
        DL_GPIO_setPins(LED1_PORT, LED1_PIN_16_PIN);
    else
        DL_GPIO_clearPins(LED1_PORT, LED1_PIN_16_PIN);
}

/* ==================== LED2 (PA28) ==================== */

void LED2(bool on)
{
    if (on)
        DL_GPIO_setPins(LED2_PORT, LED2_PIN_17_PIN);
    else
        DL_GPIO_clearPins(LED2_PORT, LED2_PIN_17_PIN);
}

/* ==================== 按键 key1 (PB6) / key2 (PB23) ==================== */

/**
 * @brief  读取 key1（低电平 = 按下）
 */
bool KEY1(void)
{
    return (DL_GPIO_readPins(key1_PORT, key1_PIN_9_PIN) == 0);
}

/**
 * @brief  读取 key2（低电平 = 按下）
 */
bool KEY2(void)
{
    return (DL_GPIO_readPins(key2_PORT, key2_PIN_10_PIN) == 0);
}
