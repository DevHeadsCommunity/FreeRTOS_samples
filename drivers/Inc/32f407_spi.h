#ifndef INC_32f407_SPI_H_
#define INC_32f407_SPI_H_

#include "../../CMSIS/Inc/stm32f407xx.h"


// Configuration struct for SPIx Peripheral
typedef struct
{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BUSConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
	uint8_t SPI_MSBFIRST;

}SPI_Config_t;


//Handle struct for SPIx Peripheral
typedef struct
{
	SPI_TypeDef *pSPIx;
	SPI_Config_t SPIConfig;
	//SPI State
	uint8_t 	*pTxBuffer;
	uint8_t 	*pRxBuffer;
	uint32_t 	TxLen;
	uint32_t 	RxLen;
	uint8_t 	TxState;
	uint8_t 	RxState;
}SPI_Handle_t;

// SPI Application states
#define SPI_READY			0
#define SPI_BUSY_IN_RX		1
#define SPI_BUSY_IN_TX		2

// SPI APP EVENTS
#define SPI_EVENT_TX_CMPLT	1
#define SPI_EVENT_RX_CMPLT	2
#define SPI_EVENT_OVR_ERR	3


// Device Modes
#define SPI_DEVICE_MODE_MASTER	1
#define SPI_DEVICE_MODE_SLAVE 	0
// SPI Bus Modes
#define SPI_BUS_CONFIG_FD				1
#define SPI_BUS_CONFIG_HD				2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY	3
// SPI CLock SPeed
#define SPI_SCLK_SPEED_DIV2					0
#define SPI_SCLK_SPEED_DIV4					1
#define SPI_SCLK_SPEED_DIV8					2
#define SPI_SCLK_SPEED_DIV16				3
#define SPI_SCLK_SPEED_DIV32				4
#define SPI_SCLK_SPEED_DIV264				5
#define SPI_SCLK_SPEED_DIV64				5
#define SPI_SCLK_SPEED_DIV128				6
#define SPI_SCLK_SPEED_DIV256				7

//SPI DFF
#define SPI_DFF_8BITS	0
#define SPI_DFF_16BITS	1
//SPI CPOL
#define SPI_CPOL_HIGH	1
#define SPI_CPOL_LOW	0
//SPI CPHA
#define SPI_CPHA_HIGH	1
#define SPI_CPHA_LOW	0
//SPI SSM
#define SPI_SSM_EN		1
#define SPI_SSM_DI		0


//APIs
// Init and DeInit
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_TypeDef *pSPIx);

//ClockSetup
void SPI_PCLK_CTRL(SPI_TypeDef *pSPIx, uint8_t EnorDi);

// Data TX and RX
uint8_t SPI_SendData(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveData(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);
void SPI_ClearOVRFlag(SPI_Handle_t *pSPIHandle);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);
void SPI_SSOEConfig(SPI_TypeDef *pSPIx, uint8_t EnorDi);
void SPI_IRQHandling(SPI_Handle_t *pHandle);
//application callback
void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEv);
#endif