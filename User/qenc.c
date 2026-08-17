#include "User.h"

#define ENCODER_COUNT 4

volatile int32_t g_qencCount[ENCODER_COUNT] = {0};
volatile int32_t g_qencSpeed[ENCODER_COUNT] = {0};
volatile int32_t g_lastCount[ENCODER_COUNT] = {0};

#define ENC1_A_PIN B3_PIN_2_PIN
#define ENC1_A_PORT B3_PORT
#define ENC1_B_PIN B4_PIN_3_PIN
#define ENC1_B_PORT B4_PORT

#define ENC2_A_PIN B1_PIN_0_PIN
#define ENC2_A_PORT B1_PORT
#define ENC2_B_PIN B2_PIN_1_PIN
#define ENC2_B_PORT B2_PORT

#define ENC3_A_PIN B5_PIN_4_PIN
#define ENC3_A_PORT B5_PORT
#define ENC3_B_PIN B6_PIN_5_PIN
#define ENC3_B_PORT B6_PORT

#define ENC4_A_PIN B7_PIN_6_PIN
#define ENC4_A_PORT B7_PORT
#define ENC4_B_PIN B8_PIN_7_PIN
#define ENC4_B_PORT B8_PORT

void QENC_Init(void)
{
    uint8_t i;
    for (i = 0; i < ENCODER_COUNT; i++) {
        g_qencCount[i] = 0;
        g_qencSpeed[i] = 0;
        g_lastCount[i] = 0;
    }

    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_ClearPendingIRQ(GPIOB_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOB_INT_IRQn);

    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

int32_t QENC_GetCount(uint8_t index)
{
    if (index < ENCODER_COUNT) {
        /* 电机2编码器反相 */
        return (index == 1) ? -g_qencCount[index] : g_qencCount[index];
    }
    return 0;
}

int32_t QENC_GetSpeed(uint8_t index)
{
    if (index < ENCODER_COUNT) {
        /* 电机2编码器反相 */
        return (index == 1) ? -g_qencSpeed[index] : g_qencSpeed[index];
    }
    return 0;
}

static void updateEncoder(uint8_t encIdx, uint32_t aPin, uint32_t bPin, GPIO_Regs *aPort, GPIO_Regs *bPort)
{
    uint32_t bVal = DL_GPIO_readPins(bPort, bPin);
    if (bVal) {
        g_qencCount[encIdx]++;
    } else {
        g_qencCount[encIdx]--;
    }
}

void GROUP1_IRQHandler(void)
{
    uint32_t iidx = DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1);

    if (iidx == DL_INTERRUPT_GROUP1_IIDX_GPIOA) {
        uint32_t status = DL_GPIO_getEnabledInterruptStatus(GPIOA,
            ENC2_A_PIN | ENC3_A_PIN | ENC3_B_PIN | ENC4_A_PIN | ENC4_B_PIN);

        if (status & ENC2_A_PIN) {
            /* ENC2 原始方向(不改ISR, 不计数的根因) */
            uint32_t bVal = DL_GPIO_readPins(ENC2_B_PORT, ENC2_B_PIN);
            if (bVal) {
                g_qencCount[1]--;
            } else {
                g_qencCount[1]++;
            }
        }
        if (status & ENC3_A_PIN) {
            updateEncoder(2, ENC3_A_PIN, ENC3_B_PIN, ENC3_A_PORT, ENC3_B_PORT);
        }
        if (status & ENC3_B_PIN) {
            uint32_t aVal = DL_GPIO_readPins(ENC3_A_PORT, ENC3_A_PIN);
            if (aVal) {
                g_qencCount[2]--;
            } else {
                g_qencCount[2]++;
            }
        }
        if (status & ENC4_A_PIN) {
            /* ENC4 */
            uint32_t bVal = DL_GPIO_readPins(ENC4_B_PORT, ENC4_B_PIN);
            if (bVal) {
                g_qencCount[3]++;
            } else {
                g_qencCount[3]--;
            }
        }
        if (status & ENC4_B_PIN) {
            /* ENC4 */
            uint32_t aVal = DL_GPIO_readPins(ENC4_A_PORT, ENC4_A_PIN);
            if (aVal) {
                g_qencCount[3]--;
            } else {
                g_qencCount[3]++;
            }
        }

        DL_GPIO_clearInterruptStatus(GPIOA, status);
    }

    if (iidx == DL_INTERRUPT_GROUP1_IIDX_GPIOB) {
        uint32_t status = DL_GPIO_getEnabledInterruptStatus(GPIOB,
            ENC1_A_PIN | ENC1_B_PIN | ENC2_B_PIN);

        if (status & ENC1_A_PIN) {
            /* ENC1 */
            uint32_t bVal = DL_GPIO_readPins(ENC1_B_PORT, ENC1_B_PIN);
            if (bVal) {
                g_qencCount[0]--;
            } else {
                g_qencCount[0]++;
            }
        }
        if (status & ENC1_B_PIN) {
            /* ENC1 */
            uint32_t aVal = DL_GPIO_readPins(ENC1_A_PORT, ENC1_A_PIN);
            if (aVal) {
                g_qencCount[0]++;
            } else {
                g_qencCount[0]--;
            }
        }
        if (status & ENC2_B_PIN) {
            /* ENC2 */
            uint32_t aVal = DL_GPIO_readPins(ENC2_A_PORT, ENC2_A_PIN);
            if (aVal) {
                g_qencCount[1]++;
            } else {
                g_qencCount[1]--;
            }
        }

        DL_GPIO_clearInterruptStatus(GPIOB, status);
    }
}


