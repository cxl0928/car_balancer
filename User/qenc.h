#ifndef _QENC_H_
#define _QENC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODER_COUNT 4

/* 编码器全局变量 (供 pid.c 定时器中断访问) */
extern volatile int32_t g_qencCount[ENCODER_COUNT];
extern volatile int32_t g_qencSpeed[ENCODER_COUNT];
extern volatile int32_t g_lastCount[ENCODER_COUNT];

void QENC_Init(void);

int32_t QENC_GetCount(uint8_t index);

int32_t QENC_GetSpeed(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* _QENC_H_ */
