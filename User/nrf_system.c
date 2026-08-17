/**
 * @file    nrf_system.c
 * @brief   NRF24L01 通信封装层
 *
 *         底层: NRF24L01.c (江协科技 SPI 驱动)
 *         本层: 屏蔽底层细节, 提供 init / send / receive 统一接口
 *               "icmok" 握手检测自动置位 nrf_icm_ready
 */

#include "nrf_system.h"
#include "NRF24L01.h"
#include "Delay.h"
#include <stdio.h>


/* ========================================================================
 *  常量 & 全局标志
 * ======================================================================== */

/** 连续错误次数阈值, 超过则复位模块 */
#define NRF_ERR_THRESHOLD   10

/** 握手魔数 "icmok" (5 字节) */
static const uint8_t ICM_MAGIC[5] = {'i', 'c', 'm', 'o', 'k'};

/** 收到 "icmok" 后置 1, 由 sys_ctrl 读取并清零 */
volatile uint8_t nrf_icm_ready = 0;


/* ========================================================================
 *  模块自检
 * ======================================================================== */

/**
 * @brief  SPI 诊断 — 读 CONFIG 寄存器验证模块是否在线
 * @return 1=在线, 0=离线 (寄存器读回 0x00 或 0xFF)
 */
static int nrf_system_check(void)
{
    uint8_t cfg = NRF24L01_ReadReg(0x00);
    if (cfg == 0x00 || cfg == 0xFF)
        return 0;
    return 1;
}


/* ========================================================================
 *  初始化 (带保护)
 * ======================================================================== */

/**
 * @brief  NRF24L01 初始化 — 模块不在线则停机
 */
void nrf_system_init(void)
{
    NRF24L01_Init();
    delay_ms(5);

    if (!nrf_system_check()) {
        printf("NRF INIT FAIL! No module detected. Halted.\r\n");
        while (1);
    }
    printf("NRF OK (CFG=0x%02X)\r\n", NRF24L01_ReadReg(0x00));
}


/* ========================================================================
 *  获取数据包指针
 * ======================================================================== */

/**
 * @brief  获取发送缓冲区指针 (长度 NRF_PACKET_SIZE)
 */
uint8_t *nrf_system_tx_packet(void)
{
    return NRF24L01_TxPacket;
}

/**
 * @brief  获取接收缓冲区指针 (长度 NRF_PACKET_SIZE)
 */
uint8_t *nrf_system_rx_packet(void)
{
    return NRF24L01_RxPacket;
}


/* ========================================================================
 *  发送
 * ======================================================================== */

/**
 * @brief  发送数据 (填充 tx_packet 后调用)
 * @return NRF_SEND_OK / NRF_SEND_NO_ACK / NRF_SEND_ERR
 */
uint8_t nrf_system_send(void)
{
    uint8_t ret = NRF24L01_Send();

    if      (ret == 1) return NRF_SEND_OK;
    else if (ret == 2) return NRF_SEND_NO_ACK;
    else               return NRF_SEND_ERR;
}


/* ========================================================================
 *  接收 (非阻塞)
 * ======================================================================== */

/**
 * @brief  非阻塞接收 — 收到 "icmok" 自动置位 nrf_icm_ready
 * @return NRF_RECV_OK / NRF_RECV_NONE / NRF_RECV_ERR
 */
uint8_t nrf_system_receive(void)
{
    uint8_t ret = NRF24L01_Receive();

    if (ret == 1) {
        /* 收到数据 — 检查是否为 "icmok" */
        uint8_t *rx   = nrf_system_rx_packet();
        uint8_t  match = 1;
        for (uint8_t i = 0; i < 5; i++) {
            if (rx[i] != ICM_MAGIC[i]) { match = 0; break; }
        }
        if (match) {
            nrf_icm_ready = 1;
        }
        return NRF_RECV_OK;
    }
    else if (ret == 0) return NRF_RECV_NONE;
    else               return NRF_RECV_ERR;
}
