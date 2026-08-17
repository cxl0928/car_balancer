#ifndef _PWM_H_
#define _PWM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== PWM 周期（与SysConfig一致） ==================== */
#define PWM_PERIOD          1000    /* 周期1000，10MHz时钟 → 10kHz PWM频率 */

/* ==================== 函数声明 ==================== */

void PWM(uint8_t Passage, int16_t duty);
#ifdef __cplusplus
}
#endif

#endif /* _PWM_H_ */
