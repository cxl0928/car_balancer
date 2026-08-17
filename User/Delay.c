/**
 * @file    Delay.c
 * @brief   微秒 / 毫秒级延时 (CPU 空循环)
 *
 *         基于 DriverLib delay_cycles(), CPUCLK_FREQ = 32MHz
 */

#include "User.h"

/* ========================================================================
 *  微秒延时
 * ======================================================================== */

/**
 * @brief  微秒级延时 (CPU 空循环)
 * @param  us  微秒数 (0 则直接返回)
 */
void delay_us(uint32_t us)
{
    if (us != 0) {
        delay_cycles(us * (CPUCLK_FREQ / 1000000UL));
    }
}

/* ========================================================================
 *  毫秒延时
 * ======================================================================== */

/**
 * @brief  毫秒级延时 (CPU 空循环, 分段防止 32bit 溢出)
 *
 *         >100ms 时分段处理, 每段 100ms
 *
 * @param  ms  毫秒数 (0 则直接返回)
 */
void delay_ms(uint32_t ms)
{
    while (ms > 100) {
        delay_cycles(100UL * (CPUCLK_FREQ / 1000UL));
        ms -= 100;
    }
    if (ms != 0) {
        delay_cycles(ms * (CPUCLK_FREQ / 1000UL));
    }
}
