#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 *  通道定义
 * ======================================================================== */

#define ENCODER_CHANNEL_COUNT   8   /**< 灰度传感器通道数 */

/* ========================================================================
 *  全局变量
 * ======================================================================== */

extern uint16_t g_encoderValues[ENCODER_CHANNEL_COUNT];        /**< 8通道 ADC 原始值 */
extern uint16_t g_encoderThresholds[ENCODER_CHANNEL_COUNT];    /**< 8通道二值化阈值    */

/* ========================================================================
 *  函数声明
 * ======================================================================== */

void     Encoder_Init(void);
void     Encoder_SelectChannel(uint8_t ch);
uint16_t Encoder_ReadChannel(uint8_t ch);
void     Encoder_ReadAll(void);
uint16_t adc_getChannel(uint8_t ch, uint8_t num);
void     adc_readAll(uint16_t *buf, uint8_t num);
void     Encoder_Compare(uint8_t result[ENCODER_CHANNEL_COUNT]);

#ifdef __cplusplus
}
#endif

#endif /* _ENCODER_H_ */
