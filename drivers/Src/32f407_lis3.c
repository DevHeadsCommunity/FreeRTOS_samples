#include <stdio.h>
#include "../Inc/32f407_spi.h"
#include "../Inc/32f407_lis3.h"


//Globals
SPI_Handle_t SPI1Handle;
extern Lis3_Config_t Accel_1;
uint8_t SPI_Actual_RX;

uint8_t dummy_read;

uint8_t dummy_write = 0xff;



void SPI_1_Init();

/**
 * @brief Read the Sensors 8 bit temp sensor and convert the value into a signed
 * that we then return
 * 
 * @return int8_t  The Current Temprature adjusted to degrees Celsius
 */
int8_t Lis3ReadTemp(){
	uint8_t temp_value;
	Lis3WriteRead(LIS3DSH_OUT_T,  &temp_value);

	 // Convert uint8_t to int8_t
    int8_t signed_temp_value = (int8_t)temp_value;

	  // If the unsigned value is greater than 127, it should be negative in 2's complement
    if (temp_value > 127) {
        signed_temp_value -= 256;
    }

	int current_degs = signed_temp_value + 25; 

	return current_degs;
}

float Lis3ReadAxis(char axis){
	uint8_t h_address, l_address;
        switch (axis) {
        case 'x':
          h_address = LIS3DSH_OUT_X_H;
          l_address = LIS3DSH_OUT_X_L;
          break;
        case 'y':
          h_address = LIS3DSH_OUT_Y_H;
          l_address = LIS3DSH_OUT_Y_L;
          break;
        case 'z':
          h_address = LIS3DSH_OUT_Z_H;
          l_address = LIS3DSH_OUT_Z_L;
          break;
		defualt:
			printf("Invalid axis \n ");
			return -5555555;
        }
    uint8_t  axis_h, axis_l; 
	int16_t axis_data = 0;
	Lis3WriteRead( l_address,  &axis_l);
	Lis3WriteRead(h_address, &axis_h);
	axis_data =  (int16_t)(axis_h << 8 | axis_l);
	float mgdata = (float)axis_data * Accel_1.Lis3_Sensitivity / 1000;

	return mgdata;
}

/**
 * @brief Provides an Initialization API for the LI3DSH Sensor
 * 
 * @param Lis3Config - Configuration struct containing the ODR, BDU and Axes
 */
void Lis3_Init(Lis3_Config_t Lis3Config){
    //Init communication Lines
    SPI_1_Init();

    uint8_t config = Lis3Config.Lis3_DR | Lis3Config.Lis3_BDU | Lis3Config.Lis3_Axes;
    printf("This is config %#X \n", config);
    uint8_t ctrl_4_address = 0x20;
    Lis3Write(ctrl_4_address, config);

}


void Lis3Write(uint8_t address, uint8_t data) {
    //Drop PE3 to select spi 1
	GPIOE->BSRR |= GPIO_BSRR_BR_3;

	SPI_SendData(&SPI1Handle,  &address, 1);

	SPI_SendData(&SPI1Handle,  &data, 1);

    //De assert SPI1
    GPIOE->BSRR |= GPIO_BSRR_BS_3;
}

void Lis3WriteRead(uint8_t read_address, uint8_t * read_data) {
    //Drop PE3 to select spi 1
	GPIOE->BSRR |= GPIO_BSRR_BR_3;
    read_address |= 0x80;

	SPI_SendData(&SPI1Handle,  &read_address, 1);

	SPI_ReceiveData(&SPI1Handle, &dummy_read, 1);

	SPI_SendData(&SPI1Handle,  &dummy_write, 1);

	SPI_ReceiveData(&SPI1Handle, read_data, 1);
    //De assert SPI1
    GPIOE->BSRR |= GPIO_BSRR_BS_3;

}

void SPI_1_Init() {
    printf("Setting up SPI 1 to MEMS ACC \n");

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


	

	
	
	//SPI_DeInit(SPI1);

	SPI1Handle.pSPIx = SPI1;
	SPI1Handle.SPIConfig.SPI_BUSConfig = SPI_BUS_CONFIG_FD;
	SPI1Handle.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1Handle.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV16;
	SPI1Handle.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI1Handle.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI1Handle.SPIConfig.SPI_CPHA = SPI_CPHA_LOW;
	SPI1Handle.SPIConfig.SPI_SSM = SPI_SSM_EN; // software slave management for NSS pin
	SPI1Handle.SPIConfig.SPI_MSBFIRST = ASSERT;

	SPI_Init(&SPI1Handle);


	printf("SPI 1 to MEMS ACC Setup and Ready \n");

}



void SPI1_IRQHandler(){ 
	SPI_IRQHandling(&SPI1Handle);
}


