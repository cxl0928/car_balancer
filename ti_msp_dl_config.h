/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSPM0G350X
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_MSPM0G350X
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                         TIMA1
#define PWM_0_INST_IRQHandler                                   TIMA1_IRQHandler
#define PWM_0_INST_INT_IRQN                                     (TIMA1_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                             16000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_0_C0_PORT                                                 GPIOB
#define GPIO_PWM_0_C0_PIN                                         DL_GPIO_PIN_17
#define GPIO_PWM_0_C0_IOMUX                                      (IOMUX_PINCM43)
#define GPIO_PWM_0_C0_IOMUX_FUNC                     IOMUX_PINCM43_PF_TIMA1_CCP0
#define GPIO_PWM_0_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_0_C1_PORT                                                 GPIOA
#define GPIO_PWM_0_C1_PIN                                         DL_GPIO_PIN_16
#define GPIO_PWM_0_C1_IOMUX                                      (IOMUX_PINCM38)
#define GPIO_PWM_0_C1_IOMUX_FUNC                     IOMUX_PINCM38_PF_TIMA1_CCP1
#define GPIO_PWM_0_C1_IDX                                    DL_TIMER_CC_1_INDEX

/* Defines for PWM_1 */
#define PWM_1_INST                                                         TIMG8
#define PWM_1_INST_IRQHandler                                   TIMG8_IRQHandler
#define PWM_1_INST_INT_IRQN                                     (TIMG8_INT_IRQn)
#define PWM_1_INST_CLK_FREQ                                             16000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_1_C0_PORT                                                 GPIOB
#define GPIO_PWM_1_C0_PIN                                         DL_GPIO_PIN_10
#define GPIO_PWM_1_C0_IOMUX                                      (IOMUX_PINCM27)
#define GPIO_PWM_1_C0_IOMUX_FUNC                     IOMUX_PINCM27_PF_TIMG8_CCP0
#define GPIO_PWM_1_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_1_C1_PORT                                                 GPIOB
#define GPIO_PWM_1_C1_PIN                                         DL_GPIO_PIN_11
#define GPIO_PWM_1_C1_IOMUX                                      (IOMUX_PINCM28)
#define GPIO_PWM_1_C1_IOMUX_FUNC                     IOMUX_PINCM28_PF_TIMG8_CCP1
#define GPIO_PWM_1_C1_IDX                                    DL_TIMER_CC_1_INDEX

/* Defines for PWM_2 */
#define PWM_2_INST                                                         TIMA0
#define PWM_2_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWM_2_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWM_2_INST_CLK_FREQ                                             16000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_2_C0_PORT                                                 GPIOA
#define GPIO_PWM_2_C0_PIN                                          DL_GPIO_PIN_0
#define GPIO_PWM_2_C0_IOMUX                                       (IOMUX_PINCM1)
#define GPIO_PWM_2_C0_IOMUX_FUNC                      IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWM_2_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_2_C1_PORT                                                 GPIOA
#define GPIO_PWM_2_C1_PIN                                          DL_GPIO_PIN_1
#define GPIO_PWM_2_C1_IOMUX                                       (IOMUX_PINCM2)
#define GPIO_PWM_2_C1_IOMUX_FUNC                      IOMUX_PINCM2_PF_TIMA0_CCP1
#define GPIO_PWM_2_C1_IDX                                    DL_TIMER_CC_1_INDEX
/* GPIO defines for channel 2 */
#define GPIO_PWM_2_C2_PORT                                                 GPIOA
#define GPIO_PWM_2_C2_PIN                                         DL_GPIO_PIN_15
#define GPIO_PWM_2_C2_IOMUX                                      (IOMUX_PINCM37)
#define GPIO_PWM_2_C2_IOMUX_FUNC                     IOMUX_PINCM37_PF_TIMA0_CCP2
#define GPIO_PWM_2_C2_IDX                                    DL_TIMER_CC_2_INDEX
/* GPIO defines for channel 3 */
#define GPIO_PWM_2_C3_PORT                                                 GPIOA
#define GPIO_PWM_2_C3_PIN                                         DL_GPIO_PIN_12
#define GPIO_PWM_2_C3_IOMUX                                      (IOMUX_PINCM34)
#define GPIO_PWM_2_C3_IOMUX_FUNC                     IOMUX_PINCM34_PF_TIMA0_CCP3
#define GPIO_PWM_2_C3_IDX                                    DL_TIMER_CC_3_INDEX

/* Defines for PWM_3 */
#define PWM_3_INST                                                         TIMG7
#define PWM_3_INST_IRQHandler                                   TIMG7_IRQHandler
#define PWM_3_INST_INT_IRQN                                     (TIMG7_INT_IRQn)
#define PWM_3_INST_CLK_FREQ                                             32000000
/* GPIO defines for channel 1 */
#define GPIO_PWM_3_C1_PORT                                                 GPIOA
#define GPIO_PWM_3_C1_PIN                                         DL_GPIO_PIN_31
#define GPIO_PWM_3_C1_IOMUX                                       (IOMUX_PINCM6)
#define GPIO_PWM_3_C1_IOMUX_FUNC                      IOMUX_PINCM6_PF_TIMG7_CCP1
#define GPIO_PWM_3_C1_IDX                                    DL_TIMER_CC_1_INDEX



/* Defines for TIMER_0 */
#define TIMER_0_INST                                                     (TIMG6)
#define TIMER_0_INST_IRQHandler                                 TIMG6_IRQHandler
#define TIMER_0_INST_INT_IRQN                                   (TIMG6_INT_IRQn)
#define TIMER_0_INST_LOAD_VALUE                                          (7999U)




/* Defines for I2C_0 */
#define I2C_0_INST                                                          I2C1
#define I2C_0_INST_IRQHandler                                    I2C1_IRQHandler
#define I2C_0_INST_INT_IRQN                                        I2C1_INT_IRQn
#define I2C_0_BUS_SPEED_HZ                                                100000
#define GPIO_I2C_0_SDA_PORT                                                GPIOA
#define GPIO_I2C_0_SDA_PIN                                        DL_GPIO_PIN_30
#define GPIO_I2C_0_IOMUX_SDA                                      (IOMUX_PINCM5)
#define GPIO_I2C_0_IOMUX_SDA_FUNC                       IOMUX_PINCM5_PF_I2C1_SDA
#define GPIO_I2C_0_SCL_PORT                                                GPIOA
#define GPIO_I2C_0_SCL_PIN                                        DL_GPIO_PIN_29
#define GPIO_I2C_0_IOMUX_SCL                                      (IOMUX_PINCM4)
#define GPIO_I2C_0_IOMUX_SCL_FUNC                       IOMUX_PINCM4_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_11
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_10
#define GPIO_UART_0_IOMUX_RX                                     (IOMUX_PINCM22)
#define GPIO_UART_0_IOMUX_TX                                     (IOMUX_PINCM21)
#define GPIO_UART_0_IOMUX_RX_FUNC                      IOMUX_PINCM22_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                      IOMUX_PINCM21_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)




/* Defines for SPI_0 */
#define SPI_0_INST                                                         SPI1
#define SPI_0_INST_IRQHandler                                   SPI1_IRQHandler
#define SPI_0_INST_INT_IRQN                                       SPI1_INT_IRQn
#define GPIO_SPI_0_PICO_PORT                                              GPIOB
#define GPIO_SPI_0_PICO_PIN                                      DL_GPIO_PIN_22
#define GPIO_SPI_0_IOMUX_PICO                                   (IOMUX_PINCM50)
#define GPIO_SPI_0_IOMUX_PICO_FUNC                   IOMUX_PINCM50_PF_SPI1_PICO
#define GPIO_SPI_0_POCI_PORT                                              GPIOB
#define GPIO_SPI_0_POCI_PIN                                      DL_GPIO_PIN_21
#define GPIO_SPI_0_IOMUX_POCI                                   (IOMUX_PINCM49)
#define GPIO_SPI_0_IOMUX_POCI_FUNC                   IOMUX_PINCM49_PF_SPI1_POCI
/* GPIO configuration for SPI_0 */
#define GPIO_SPI_0_SCLK_PORT                                              GPIOA
#define GPIO_SPI_0_SCLK_PIN                                      DL_GPIO_PIN_17
#define GPIO_SPI_0_IOMUX_SCLK                                   (IOMUX_PINCM39)
#define GPIO_SPI_0_IOMUX_SCLK_FUNC                   IOMUX_PINCM39_PF_SPI1_SCLK
/* Defines for SPI_1 */
#define SPI_1_INST                                                         SPI0
#define SPI_1_INST_IRQHandler                                   SPI0_IRQHandler
#define SPI_1_INST_INT_IRQN                                       SPI0_INT_IRQn
#define GPIO_SPI_1_PICO_PORT                                              GPIOA
#define GPIO_SPI_1_PICO_PIN                                       DL_GPIO_PIN_9
#define GPIO_SPI_1_IOMUX_PICO                                   (IOMUX_PINCM20)
#define GPIO_SPI_1_IOMUX_PICO_FUNC                   IOMUX_PINCM20_PF_SPI0_PICO
#define GPIO_SPI_1_POCI_PORT                                              GPIOB
#define GPIO_SPI_1_POCI_PIN                                      DL_GPIO_PIN_19
#define GPIO_SPI_1_IOMUX_POCI                                   (IOMUX_PINCM45)
#define GPIO_SPI_1_IOMUX_POCI_FUNC                   IOMUX_PINCM45_PF_SPI0_POCI
/* GPIO configuration for SPI_1 */
#define GPIO_SPI_1_SCLK_PORT                                              GPIOB
#define GPIO_SPI_1_SCLK_PIN                                      DL_GPIO_PIN_18
#define GPIO_SPI_1_IOMUX_SCLK                                   (IOMUX_PINCM44)
#define GPIO_SPI_1_IOMUX_SCLK_FUNC                   IOMUX_PINCM44_PF_SPI0_SCLK



/* Defines for ADC12_0 */
#define ADC12_0_INST                                                        ADC0
#define ADC12_0_INST_IRQHandler                                  ADC0_IRQHandler
#define ADC12_0_INST_INT_IRQN                                    (ADC0_INT_IRQn)
#define ADC12_0_ADCMEM_0                                      DL_ADC12_MEM_IDX_0
#define ADC12_0_ADCMEM_0_REF                     DL_ADC12_REFERENCE_VOLTAGE_VDDA
#define ADC12_0_ADCMEM_0_REF_VOLTAGE_V                                       3.3
#define GPIO_ADC12_0_C0_PORT                                               GPIOA
#define GPIO_ADC12_0_C0_PIN                                       DL_GPIO_PIN_27



/* Defines for DMA_CH0 */
#define DMA_CH0_CHAN_ID                                                      (0)
#define ADC12_0_INST_DMA_TRIGGER                      (DMA_ADC0_EVT_GEN_BD_TRIG)



/* Port definition for Pin Group SDA */
#define SDA_PORT                                                         (GPIOB)

/* Defines for SDA_PIN: GPIOB.8 with pinCMx 25 on package pin 60 */
#define SDA_SDA_PIN_PIN                                          (DL_GPIO_PIN_8)
#define SDA_SDA_PIN_IOMUX                                        (IOMUX_PINCM25)
/* Port definition for Pin Group SCL */
#define SCL_PORT                                                         (GPIOB)

/* Defines for SCL_PIN: GPIOB.9 with pinCMx 26 on package pin 61 */
#define SCL_SCL_PIN_PIN                                          (DL_GPIO_PIN_9)
#define SCL_SCL_PIN_IOMUX                                        (IOMUX_PINCM26)
/* Port definition for Pin Group B1 */
#define B1_PORT                                                          (GPIOA)

/* Defines for PIN_0: GPIOA.13 with pinCMx 35 on package pin 6 */
// groups represented: ["B5","B6","B7","B8","B1"]
// pins affected: ["PIN_4","PIN_5","PIN_6","PIN_7","PIN_0"]
#define GPIO_MULTIPLE_GPIOA_INT_IRQN                            (GPIOA_INT_IRQn)
#define GPIO_MULTIPLE_GPIOA_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define B1_PIN_0_IIDX                                       (DL_GPIO_IIDX_DIO13)
#define B1_PIN_0_PIN                                            (DL_GPIO_PIN_13)
#define B1_PIN_0_IOMUX                                           (IOMUX_PINCM35)
/* Port definition for Pin Group B2 */
#define B2_PORT                                                          (GPIOB)

/* Defines for PIN_1: GPIOB.14 with pinCMx 31 on package pin 2 */
// groups represented: ["B3","B4","B2"]
// pins affected: ["PIN_2","PIN_3","PIN_1"]
#define GPIO_MULTIPLE_GPIOB_INT_IRQN                            (GPIOB_INT_IRQn)
#define GPIO_MULTIPLE_GPIOB_INT_IIDX            (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define B2_PIN_1_IIDX                                       (DL_GPIO_IIDX_DIO14)
#define B2_PIN_1_PIN                                            (DL_GPIO_PIN_14)
#define B2_PIN_1_IOMUX                                           (IOMUX_PINCM31)
/* Port definition for Pin Group B3 */
#define B3_PORT                                                          (GPIOB)

/* Defines for PIN_2: GPIOB.13 with pinCMx 30 on package pin 1 */
#define B3_PIN_2_IIDX                                       (DL_GPIO_IIDX_DIO13)
#define B3_PIN_2_PIN                                            (DL_GPIO_PIN_13)
#define B3_PIN_2_IOMUX                                           (IOMUX_PINCM30)
/* Port definition for Pin Group B4 */
#define B4_PORT                                                          (GPIOB)

/* Defines for PIN_3: GPIOB.26 with pinCMx 57 on package pin 28 */
#define B4_PIN_3_IIDX                                       (DL_GPIO_IIDX_DIO26)
#define B4_PIN_3_PIN                                            (DL_GPIO_PIN_26)
#define B4_PIN_3_IOMUX                                           (IOMUX_PINCM57)
/* Port definition for Pin Group B5 */
#define B5_PORT                                                          (GPIOA)

/* Defines for PIN_4: GPIOA.14 with pinCMx 36 on package pin 7 */
#define B5_PIN_4_IIDX                                       (DL_GPIO_IIDX_DIO14)
#define B5_PIN_4_PIN                                            (DL_GPIO_PIN_14)
#define B5_PIN_4_IOMUX                                           (IOMUX_PINCM36)
/* Port definition for Pin Group B6 */
#define B6_PORT                                                          (GPIOA)

/* Defines for PIN_5: GPIOA.22 with pinCMx 47 on package pin 18 */
#define B6_PIN_5_IIDX                                       (DL_GPIO_IIDX_DIO22)
#define B6_PIN_5_PIN                                            (DL_GPIO_PIN_22)
#define B6_PIN_5_IOMUX                                           (IOMUX_PINCM47)
/* Port definition for Pin Group B7 */
#define B7_PORT                                                          (GPIOA)

/* Defines for PIN_6: GPIOA.25 with pinCMx 55 on package pin 26 */
#define B7_PIN_6_IIDX                                       (DL_GPIO_IIDX_DIO25)
#define B7_PIN_6_PIN                                            (DL_GPIO_PIN_25)
#define B7_PIN_6_IOMUX                                           (IOMUX_PINCM55)
/* Port definition for Pin Group B8 */
#define B8_PORT                                                          (GPIOA)

/* Defines for PIN_7: GPIOA.24 with pinCMx 54 on package pin 25 */
#define B8_PIN_7_IIDX                                       (DL_GPIO_IIDX_DIO24)
#define B8_PIN_7_PIN                                            (DL_GPIO_PIN_24)
#define B8_PIN_7_IOMUX                                           (IOMUX_PINCM54)
/* Port definition for Pin Group PC817 */
#define PC817_PORT                                                       (GPIOB)

/* Defines for PIN_8: GPIOB.12 with pinCMx 29 on package pin 64 */
#define PC817_PIN_8_PIN                                         (DL_GPIO_PIN_12)
#define PC817_PIN_8_IOMUX                                        (IOMUX_PINCM29)
/* Port definition for Pin Group ICM42688 */
#define ICM42688_PORT                                                    (GPIOB)

/* Defines for CS: GPIOB.27 with pinCMx 58 on package pin 29 */
#define ICM42688_CS_PIN                                         (DL_GPIO_PIN_27)
#define ICM42688_CS_IOMUX                                        (IOMUX_PINCM58)
/* Port definition for Pin Group key1 */
#define key1_PORT                                                        (GPIOB)

/* Defines for PIN_9: GPIOB.6 with pinCMx 23 on package pin 58 */
#define key1_PIN_9_PIN                                           (DL_GPIO_PIN_6)
#define key1_PIN_9_IOMUX                                         (IOMUX_PINCM23)
/* Port definition for Pin Group key2 */
#define key2_PORT                                                        (GPIOB)

/* Defines for PIN_10: GPIOB.23 with pinCMx 51 on package pin 22 */
#define key2_PIN_10_PIN                                         (DL_GPIO_PIN_23)
#define key2_PIN_10_IOMUX                                        (IOMUX_PINCM51)
/* Port definition for Pin Group NRF_CSN */
#define NRF_CSN_PORT                                                     (GPIOB)

/* Defines for PIN_11: GPIOB.25 with pinCMx 56 on package pin 27 */
#define NRF_CSN_PIN_11_PIN                                      (DL_GPIO_PIN_25)
#define NRF_CSN_PIN_11_IOMUX                                     (IOMUX_PINCM56)
/* Port definition for Pin Group NRF_CE */
#define NRF_CE_PORT                                                      (GPIOB)

/* Defines for PIN_12: GPIOB.24 with pinCMx 52 on package pin 23 */
#define NRF_CE_PIN_12_PIN                                       (DL_GPIO_PIN_24)
#define NRF_CE_PIN_12_IOMUX                                      (IOMUX_PINCM52)
/* Port definition for Pin Group AD0 */
#define AD0_PORT                                                         (GPIOA)

/* Defines for PIN_13: GPIOA.26 with pinCMx 59 on package pin 30 */
#define AD0_PIN_13_PIN                                          (DL_GPIO_PIN_26)
#define AD0_PIN_13_IOMUX                                         (IOMUX_PINCM59)
/* Port definition for Pin Group AD1 */
#define AD1_PORT                                                         (GPIOB)

/* Defines for PIN_14: GPIOB.1 with pinCMx 13 on package pin 48 */
#define AD1_PIN_14_PIN                                           (DL_GPIO_PIN_1)
#define AD1_PIN_14_IOMUX                                         (IOMUX_PINCM13)
/* Port definition for Pin Group AD2 */
#define AD2_PORT                                                         (GPIOB)

/* Defines for PIN_15: GPIOB.0 with pinCMx 12 on package pin 47 */
#define AD2_PIN_15_PIN                                           (DL_GPIO_PIN_0)
#define AD2_PIN_15_IOMUX                                         (IOMUX_PINCM12)
/* Port definition for Pin Group LED1 */
#define LED1_PORT                                                        (GPIOA)

/* Defines for PIN_16: GPIOA.8 with pinCMx 19 on package pin 54 */
#define LED1_PIN_16_PIN                                          (DL_GPIO_PIN_8)
#define LED1_PIN_16_IOMUX                                        (IOMUX_PINCM19)
/* Port definition for Pin Group LED2 */
#define LED2_PORT                                                        (GPIOA)

/* Defines for PIN_17: GPIOA.28 with pinCMx 3 on package pin 35 */
#define LED2_PIN_17_PIN                                         (DL_GPIO_PIN_28)
#define LED2_PIN_17_IOMUX                                         (IOMUX_PINCM3)

/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_PWM_1_init(void);
void SYSCFG_DL_PWM_2_init(void);
void SYSCFG_DL_PWM_3_init(void);
void SYSCFG_DL_TIMER_0_init(void);
void SYSCFG_DL_I2C_0_init(void);
void SYSCFG_DL_UART_0_init(void);
void SYSCFG_DL_SPI_0_init(void);
void SYSCFG_DL_SPI_1_init(void);
void SYSCFG_DL_ADC12_0_init(void);
void SYSCFG_DL_DMA_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
