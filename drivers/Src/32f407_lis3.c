#include <stdio.h>
#include "../Inc/32f407_spi.h"
#include "../Inc/32f407_lis3.h"


//Globals
SPI_Handle_t SPI1Handle;
uint8_t SPI_Actual_RX;

uint8_t dummy_read;

uint8_t dummy_write = 0xff;



void SPI_1_Init();




void Lis3_Init(){
    //Init communication Lines
    SPI_1_Init();

    //LIS3 Setup
    //choose datarate
    //choose filter 
    //enable axes
    
}

void Lis3WriteRead(uint8_t read_address, uint8_t * read_data) {
    //Drop PE3 to select spi 1
	GPIOE->BSRR |= GPIO_BSRR_BR_3;
    read_address |= 0x80;

	delay();
	SPI_SendData(&SPI1Handle,  &read_address, 1);

	SPI_ReceiveData(&SPI1Handle, &dummy_read, 1);

	SPI_SendData(&SPI1Handle,  &dummy_write, 1);

	SPI_ReceiveData(&SPI1Handle, read_data, 1);
    delay();
    //De assert SPI1
    GPIOE->BSRR |= GPIO_BSRR_BS_3;

}

void SPI_1_Init() {

	uint32_t altfn_reg;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;

	//MOSI
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_7;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7_0;
	//get this pins altfn reg
	altfn_reg = 7;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);
	

	//MISO
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_6;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6_0;
	//get this pins altfn reg
	altfn_reg = 6;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);

	//SCLK
	GPIOA->MODER |= GPIO_MODER_MODER5_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5_0;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5_0;
	//get this pins altfn reg
	altfn_reg = 5;
	GPIOA->AFR[0] |= 5 << (altfn_reg * 4);

	//NSS
	GPIOE->MODER |= GPIO_MODER_MODER3_0;
	GPIOE->OTYPER &= ~(1 << GPIO_OTYPER_OT3_Pos);
	GPIOE->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3;
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPDR3_0;
	
	//SET PE3 to disable SPI1
	GPIOE->BSRR |= GPIO_BSRR_BS_3;
	printf("GPIOE ODR: %#X \n", GPIOE->ODR);


	

	printf("SPI 1 to MEMS ACC enabling \n");
	
	//SPI_DeInit(SPI1);

	SPI1Handle.pSPIx = SPI1;
	SPI1Handle.SPIConfig.SPI_BUSConfig = SPI_BUS_CONFIG_FD;
	SPI1Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV16;
	SPI1Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI1Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI1Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI1Handle.SPIConfig.SPI_SSM = SPI_SSM_EN; // software slave management for NSS pin
	SPI1Handle.SPIConfig.SPI_MSBFIRST = ENABLE;

	SPI_Init(&SPI1Handle);


	printf("SPI 1 to MEMS ACC enabled \n");

}



void SPI1_IRQHandler(){ 
	SPI_IRQHandling(&SPI1Handle);
}


//application callback
//Remember to check that this is SPI1 below so we dont affect the other SPI stuff
void SPI_ApplicationEventCallBack(SPI_Handle_t *pSPIHandle, uint8_t AppEv){
	// weak implementation for Application to override
	switch (AppEv) {
	case SPI_EVENT_TX_CMPLT:
		printf("We Handled SPI EVENT TX: %d \n", AppEv);
		break;
	case SPI_EVENT_RX_CMPLT:
		//if(SPI_Actual_RX){
		printf("We Handled SPI EVENT RX: %d \n", AppEv);
		printf("This is Dummy Read after RX_CPLT:  %X \n", (uint16_t)dummy_read);
		//}
		break;
	case SPI_EVENT_OVR_ERR:
		printf("We Handled SPI EVENT OVR: %d \n", AppEv);
		break;
	}
}