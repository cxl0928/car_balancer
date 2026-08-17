/**
 * @file    encoder.c
 * @brief   8 通道灰度传感器驱动 (ADC + 模拟开关)
 *
 *         硬件: 3 个 GPIO 控制 4051 模拟开关选通 8 通道,
 *                ADC12_0 读取电压, 与阈值比较输出二值化结果
 *         用途: 循迹模式下的黑线检测
 */

#include "User.h"

/* ========================================================================
 *  全局变量
 * ======================================================================== */

/** 8 通道 ADC 原始值 (Encoder_ReadAll 更新) */
uint16_t g_encoderValues[ENCODER_CHANNEL_COUNT];

/** 8 通道二值化阈值 (val > 阈值 → 1=白, val ≤ 阈值 → 0=黑) */
uint16_t g_encoderThresholds[ENCODER_CHANNEL_COUNT] = {
    1600, 1600, 2000, 2000, 2250, 1700, 1000, 1400
};


/* ========================================================================
 *  初始化
 * ======================================================================== */

/**
 * @brief  灰度传感器初始化 — 启动 ADC 连续转换
 *
 *         采样时间: 100+1 ≈ 3.16us @ 32MHz ADC 时钟
 */
void Encoder_Init(void)
{
    DL_ADC12_setSampleTime0(ADC12_0_INST, 100);
    DL_ADC12_enableConversions(ADC12_0_INST);
    DL_ADC12_startConversion(ADC12_0_INST);
}


/* ========================================================================
 *  模拟开关通道选择 (4051: AD2/AD1/AD0 输出 3-bit 地址)
 * ======================================================================== */

/**
 * @brief  选择 4051 模拟开关通道
 *
 *         AD0=PA26 (bit0), AD1=PB1 (bit1), AD2=PB0 (bit2)
 *
 * @param  ch  通道号 0~7
 */
void Encoder_SelectChannel(uint8_t ch)
{
    if (ch & 0x01)
        DL_GPIO_setPins  (AD0_PORT, AD0_PIN_13_PIN);
    else
        DL_GPIO_clearPins(AD0_PORT, AD0_PIN_13_PIN);

    if (ch & 0x02)
        DL_GPIO_setPins  (AD1_PORT, AD1_PIN_14_PIN);
    else
        DL_GPIO_clearPins(AD1_PORT, AD1_PIN_14_PIN);

    if (ch & 0x04)
        DL_GPIO_setPins  (AD2_PORT, AD2_PIN_15_PIN);
    else
        DL_GPIO_clearPins(AD2_PORT, AD2_PIN_15_PIN);
}


/* ========================================================================
 *  ADC 读取
 * ======================================================================== */

/**
 * @brief  选择通道并读取单次 ADC 值
 *
 *         切换通道后等待 50us 稳定, 再读 ADC 结果寄存器
 *
 * @param  ch  通道号 0~7
 * @return ADC 原始值 (12bit, 0~4095)
 */
uint16_t Encoder_ReadChannel(uint8_t ch)
{
    Encoder_SelectChannel(ch);
    delay_us(50);

    return ADC12_0_INST->ULLMEM.MEMRES[ADC12_0_ADCMEM_0];
}

/**
 * @brief  扫描全部 8 通道, 结果存入 g_encoderValues[]
 */
void Encoder_ReadAll(void)
{
    for (uint8_t ch = 0; ch < ENCODER_CHANNEL_COUNT; ch++) {
        g_encoderValues[ch] = Encoder_ReadChannel(ch);
    }
}


/* ========================================================================
 *  多次平均读取
 * ======================================================================== */

/**
 * @brief  指定通道多次采样取平均
 *
 *         切换通道后延时 50us, 后续每次采样间隔 30us
 *
 * @param  ch   通道号 0~7
 * @param  num  采样次数
 * @return 平均值 (uint16_t)
 */
uint16_t adc_getChannel(uint8_t ch, uint8_t num)
{
    uint32_t sum = 0;

    Encoder_SelectChannel(ch);
    delay_us(50);   /* 模拟开关稳定 + 第一次转换 */

    for (uint8_t i = 0; i < num; i++) {
        delay_us(30);   /* 等待下一次 ADC 转换完成 */
        sum += ADC12_0_INST->ULLMEM.MEMRES[ADC12_0_ADCMEM_0];
    }

    return (uint16_t)(sum / num);
}

/**
 * @brief  全通道多次平均读取
 * @param  buf  存放结果的数组 (长度 ≥ ENCODER_CHANNEL_COUNT)
 * @param  num  每通道采样次数
 */
void adc_readAll(uint16_t *buf, uint8_t num)
{
    for (uint8_t ch = 0; ch < ENCODER_CHANNEL_COUNT; ch++) {
        buf[ch] = adc_getChannel(ch, num);
    }
}


/* ========================================================================
 *  阈值比较 (二值化)
 * ======================================================================== */

/**
 * @brief  读取全部 8 通道并与阈值比较, 输出 0/1 二值化结果
 *
 *         val > 阈值 → 1 (白),  val ≤ 阈值 → 0 (黑)
 *
 * @param  result  存放 0/1 结果的数组 (长度 ≥ ENCODER_CHANNEL_COUNT)
 */
void Encoder_Compare(uint8_t result[ENCODER_CHANNEL_COUNT])
{
    for (uint8_t ch = 0; ch < ENCODER_CHANNEL_COUNT; ch++) {
        uint16_t val = Encoder_ReadChannel(ch);
        result[ch] = (val > g_encoderThresholds[ch]) ? 1 : 0;
    }
}
