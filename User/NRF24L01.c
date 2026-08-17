/***************************************************************************************
  * 本程序由江协科技创建并免费开源共享
  * 你可以任意查看、使用和修改，并应用到自己的项目之中
  * 程序版权归江协科技所有，任何人或组织不得将其据为己有
  * 
  * 程序名称：				NRF24L01无线通信模块驱动程序
  * 程序创建时间：			2025.6.9
  * 当前程序版本：			V1.0
  * 当前版本发布时间：		2025.6.9
  * 
  * 江协科技官方网站：		jiangxiekeji.com
  * 江协科技官方淘宝店：	jiangxiekeji.taobao.com
  * 程序介绍及更新动态：	jiangxiekeji.com/tutorial/nrf24l01.html
  * 
  * [移植 MSPM0] 硬件 SPI1 (PA17/SCLK, PB22/PICO, PB21/POCI)
  *               GPIO 控制: PB25/CSN, PB24/CE
  ***************************************************************************************
  */

#include "../ti_msp_dl_config.h"
#include "NRF24L01_Define.h"

/*全局变量*********************/

/*发送部分*/
uint8_t NRF24L01_TxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};		//发送地址，固定5字节
#define NRF24L01_TX_PACKET_WIDTH		5							//发送数据包宽度，范围：1~32字节
uint8_t NRF24L01_TxPacket[NRF24L01_TX_PACKET_WIDTH];				//发送数据包

/*接收部分*/
uint8_t NRF24L01_RxAddress[5] = {0x11, 0x22, 0x33, 0x44, 0x55};		//接收通道0地址，固定5字节
#define NRF24L01_RX_PACKET_WIDTH		5							//接收通道0数据包宽度，范围：1~32字节
uint8_t NRF24L01_RxPacket[NRF24L01_RX_PACKET_WIDTH];				//接收数据包

/*********************全局变量*/


/*引脚配置*********************/

void NRF24L01_W_CE(uint8_t BitValue)
{
	if (BitValue)
		DL_GPIO_setPins(NRF_CE_PORT, NRF_CE_PIN_12_PIN);
	else
		DL_GPIO_clearPins(NRF_CE_PORT, NRF_CE_PIN_12_PIN);
}

void NRF24L01_W_CSN(uint8_t BitValue)
{
	if (BitValue)
		DL_GPIO_setPins(NRF_CSN_PORT, NRF_CSN_PIN_11_PIN);
	else
		DL_GPIO_clearPins(NRF_CSN_PORT, NRF_CSN_PIN_11_PIN);
}

void NRF24L01_GPIO_Init(void)
{
	/* SysConfig 已初始化 CE(PB24 输出 LOW) 和 CSN(PB25 输出 HIGH) */
	NRF24L01_W_CE(0);
	NRF24L01_W_CSN(1);
}

/*********************引脚配置*/


/*通信协议*********************/

/**
 * 函    数：SPI交换一个字节（硬件SPI1）
 * 参    数：Byte 要发送的一个字节数据
 * 返 回 值：接收得到的一个字节数据
 */
static uint8_t NRF24L01_SPI_SwapByte(uint8_t Byte)
{
	DL_SPI_transmitDataBlocking8(SPI_1_INST, Byte);
	return DL_SPI_receiveDataBlocking8(SPI_1_INST);
}

/*********************通信协议*/


/*指令实现*********************/

uint8_t NRF24L01_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	Data = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	NRF24L01_W_CSN(1);
	return Data;
}

void NRF24L01_ReadRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_REGISTER | RegAddress);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	NRF24L01_SPI_SwapByte(Data);
	NRF24L01_W_CSN(1);
}

void NRF24L01_WriteRegs(uint8_t RegAddress, uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_REGISTER | RegAddress);
	for (i = 0; i < Count; i ++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_ReadRxPayload(uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_R_RX_PAYLOAD);
	for (i = 0; i < Count; i ++)
	{
		DataArray[i] = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_WriteTxPayload(uint8_t *DataArray, uint8_t Count)
{
	uint8_t i;
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_W_TX_PAYLOAD);
	for (i = 0; i < Count; i ++)
	{
		NRF24L01_SPI_SwapByte(DataArray[i]);
	}
	NRF24L01_W_CSN(1);
}

void NRF24L01_FlushTx(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_TX);
	NRF24L01_W_CSN(1);
}

void NRF24L01_FlushRx(void)
{
	NRF24L01_W_CSN(0);
	NRF24L01_SPI_SwapByte(NRF24L01_FLUSH_RX);
	NRF24L01_W_CSN(1);
}

uint8_t NRF24L01_ReadStatus(void)
{
	uint8_t Status;
	NRF24L01_W_CSN(0);
	Status = NRF24L01_SPI_SwapByte(NRF24L01_NOP);
	NRF24L01_W_CSN(1);
	return Status;
}

/*********************指令实现*/


/*功能函数*********************/

void NRF24L01_PowerDown(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	if (Config == 0xFF) {return;}
	Config &= ~0x02;
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

void NRF24L01_StandbyI(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	if (Config == 0xFF) {return;}
	Config |= 0x02;
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
}

void NRF24L01_Rx(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	if (Config == 0xFF) {return;}
	Config |= 0x03;
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
	NRF24L01_W_CE(1);
}

void NRF24L01_Tx(void)
{
	uint8_t Config;
	NRF24L01_W_CE(0);
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);
	if (Config == 0xFF) {return;}
	Config |= 0x02;
	Config &= ~0x01;
	NRF24L01_WriteReg(NRF24L01_CONFIG, Config);
	NRF24L01_W_CE(1);
}

void NRF24L01_Init(void)
{
	NRF24L01_GPIO_Init();

	NRF24L01_WriteReg(NRF24L01_CONFIG, 0x08);
	NRF24L01_WriteReg(NRF24L01_EN_AA, 0x3F);
	NRF24L01_WriteReg(NRF24L01_EN_RXADDR, 0x01);
	NRF24L01_WriteReg(NRF24L01_SETUP_AW, 0x03);
	NRF24L01_WriteReg(NRF24L01_SETUP_RETR, 0x03);
	NRF24L01_WriteReg(NRF24L01_RF_CH, 0x02);
	NRF24L01_WriteReg(NRF24L01_RF_SETUP, 0x0E);
	NRF24L01_WriteReg(NRF24L01_RX_PW_P0, NRF24L01_RX_PACKET_WIDTH);
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
	NRF24L01_FlushTx();
	NRF24L01_FlushRx();
	NRF24L01_WriteReg(NRF24L01_STATUS, 0x70);
	NRF24L01_Rx();
}

uint8_t NRF24L01_Send(void)
{
	uint8_t Status;
	uint8_t SendFlag;
	uint32_t Timeout;

	NRF24L01_WriteRegs(NRF24L01_TX_ADDR, NRF24L01_TxAddress, 5);
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_TxAddress, 5);
	NRF24L01_WriteTxPayload(NRF24L01_TxPacket, NRF24L01_TX_PACKET_WIDTH);
	NRF24L01_Tx();

	Timeout = 10000;
	while (1)
	{
		Status = NRF24L01_ReadStatus();
		Timeout --;
		if (Timeout == 0)
		{
			SendFlag = 4;
			NRF24L01_Init();
			break;
		}
		if ((Status & 0x30) == 0x30)
		{
			SendFlag = 3;
			NRF24L01_Init();
			break;
		}
		else if ((Status & 0x10) == 0x10)
		{
			SendFlag = 2;
			NRF24L01_Init();
			break;
		}
		else if ((Status & 0x20) == 0x20)
		{
			SendFlag = 1;
			break;
		}
	}

	NRF24L01_WriteReg(NRF24L01_STATUS, 0x30);
	NRF24L01_FlushTx();
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
	NRF24L01_Rx();

	return SendFlag;
}

uint8_t NRF24L01_Receive(void)
{
	uint8_t Status, Config;
	uint8_t ReceiveFlag;

	Status = NRF24L01_ReadStatus();
	Config = NRF24L01_ReadReg(NRF24L01_CONFIG);

	if ((Config & 0x02) == 0x00)
	{
		ReceiveFlag = 3;
		NRF24L01_Init();
	}
	else if ((Status & 0x30) == 0x30)
	{
		ReceiveFlag = 2;
		NRF24L01_Init();
	}
	else if ((Status & 0x40) == 0x40)
	{
		ReceiveFlag = 1;
		NRF24L01_ReadRxPayload(NRF24L01_RxPacket, NRF24L01_RX_PACKET_WIDTH);
		NRF24L01_WriteReg(NRF24L01_STATUS, 0x40);
		NRF24L01_FlushRx();
	}
	else
	{
		ReceiveFlag = 0;
	}

	return ReceiveFlag;
}

void NRF24L01_UpdateRxAddress(void)
{
	NRF24L01_WriteRegs(NRF24L01_RX_ADDR_P0, NRF24L01_RxAddress, 5);
}

/*********************功能函数*/
