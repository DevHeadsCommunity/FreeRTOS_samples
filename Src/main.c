/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

 //for print f
#include <stdint.h>
#include <stdio.h>

#include "../CMSIS/Inc/main.h"
#include "../CMSIS/Inc/stm32f407xx.h"
#include "../freertos/source/include/FreeRTOS.h"
#include "../freertos/source/include/task.h"


Lis3_Config_t Accel_1;
volatile uint8_t SHOULD_READ = 1;
I2C_Handle_t I2C1Handle;
#define device_id 0xAA;

void prvSetupHardware(void);
void vHeartBeat(void * pvParams);










int main(void)
{	
	// Setup our Hardware
	prvSetupHardware();

	



	 // Should be in rtc task 

	uint8_t code = 0x00;
	uint8_t addr = 0x68;
	uint8_t rx_buf[3];

	uint8_t rxes[2] = {0x00, 0x7F};
	// Send some data
	I2C_MasterSendData(&I2C1Handle, rxes, 2, addr, REFUTE);

	for(uint32_t i = 0; i < 200000; i++){}
	
	I2C_MasterReceiveData(&I2C1Handle, rx_buf, 3, addr, REFUTE);
	

	/*end i2c*/

	/* Create tasks */
	xTaskCreate(vHeartBeat,
				"Heart Beat",
				50,
				NULL, 
				1, 
				NULL);	 

	/*
	start freertos
	Once this is started we have handed over control to freertos
	*/
	
	vTaskStartScheduler();

	// the following code is unreachable.
	// Should be moved into specific tasks.

	
	
	while(1) {

		// Should be in Accel task
		// delay before reading this.
		float x_reading = Lis3ReadAxis('x');
		//printf("Combined X axis movement in mg: %.1f and  \n", x_reading);

		if(SHOULD_READ == 1){
			

		
			
			int8_t today_temp = Lis3ReadTemp();
			printf("Temp is %dC Degrees Celsius \n", today_temp);
			
			SHOULD_READ = 0;


			//AdcReadChannel(16);
		}
		
		
	}

	return 0;
}


void prvSetupPA6asPWM(void) {

	//Enable IO A & set PA6 as output
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	//AF Mode
	GPIOA->MODER |= GPIO_MODER_MODER6_1;
	//MAX SPEED
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED6;
	//AF2 Mode
	GPIOD->AFR[0] = GPIO_AFRL_AFRL6_2;


	//PWM on Timer3 - Channel 1 
	RCC ->APB1ENR |= RCC_APB1ENR_TIM3EN;

	TIM3->CR1 &= ~TIM_CR1_CEN;

	//Enable ARPE
	TIM3->CR1 |= TIM_CR1_ARPE;

	//dont pre scale this clock so it runs at 16 MHZ like sys
	TIM3->PSC = 0;

	//set the arr for a 300khz period
	TIM3->ARR = 53;

	//use the ccr reg for duty of
	TIM3->CCR1 = 2;

	//set the timer to pwm mode 1 on channel 1
	TIM3->CCMR1 |=  6U << TIM_CCMR1_OC1M_Pos;

	//Enable channel 1 preload
	TIM3->CCMR1 |= TIM_CCMR1_OC1PE;

	//Enable compare mode
	TIM3->CCER |= TIM_CCER_CC1E;

	//load prescaler and enable update events
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->CR1 |= TIM_CR1_CEN;

	//wait for update evebt
	while(!(TIM3->SR & TIM_SR_UIF));

	// Clear Interrupt Flag
	TIM3->SR &= ~(TIM_SR_UIF);
	
	//Reset the counter
	TIM3->CNT = 0;

}

/**
 * @brief prvSetupHardware
 *  Does all the hardware set up for the application 
 *	Including, clock speed setup,  driver initializations, mode inits
 */
void prvSetupHardware(void){

	//enable FPU
	SCB->CPACR |= ((3UL << 20U)|(3UL << 22U));

	// log out sys speed
	uint32_t rcc_speed =   RCC_GetPCLK1Value();

	printf("System booted with Default clock of: %d MHz \n", rcc_speed);

	


	/*LIS3 Setup*/
    //choose datarate, filter, axes
	Accel_1.Lis3_DR = 0x60;
	Accel_1.Lis3_BDU = 0x0;
	Accel_1.Lis3_Axes = 0x7;
	Accel_1.Lis3_Sensitivity = SENSITIVITY_2G;
	/* END LIS3 Setup*/

	/*ADC SETup*/

	AdcInit();

	/* End ADC SETUP */

	

	/*Use SPI to set up Accelerometer*/
	uint8_t read_address = 0x0F;
	uint8_t read_data = 0xAA;
	uint8_t read_settings = 0xFF;
	uint8_t ctrl_4 = 0x20;
	
	Lis3_Init(Accel_1);
	
	Lis3WriteRead(read_address, &read_data);
	printf("ID Data: %#X \n", read_data);
	Lis3WriteRead(ctrl_4, &read_settings);
	printf("ODR and Axes Config Data: %#X \n", read_settings);
	/*End Accelerometer set up*/
	

	//Setup 12c for RTC
	/*I2c*/
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
	uint32_t altfn_reg;

	//get this pins altfn reg
	altfn_reg = 6%8;
	GPIOB->AFR[0] |= 4 << (altfn_reg * 4);
	//Set up pins
	GPIOB->MODER |= GPIO_MODER_MODER6_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT6;
	// Very fast speed for i2c
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR6_0;
	
	
	//get this pins altfn reg
	altfn_reg = 7%8;
	GPIOB->AFR[0] |= 4 << (altfn_reg * 4);

	GPIOB->MODER |= GPIO_MODER_MODER7_1;
	GPIOB->OTYPER |= GPIO_OTYPER_OT7;
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;
	

	

	//Init i2c1
	I2C1Handle.pI2Cx = I2C1;
	I2C1Handle.I2C_Config.I2C_ACKControl = ASSERT;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = device_id;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;
	I2C_Init(&I2C1Handle);
	printf("GPIO setup on port b pin 6 scl pin 7 sda \n");

	/* End i2c init for RTC*/

	//Set up Blue LED for heart beat
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	GPIOD->MODER |= GPIO_MODER_MODER15_0;
	GPIOD->MODER &= ~GPIO_MODER_MODER15_1;
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT_15;
	GPIOD->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR15_0;
	GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15_1;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_0;
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPDR15_1;




}

void vHeartBeat(void * pvParams) {
	for (;;)
	{
		GPIOD->BSRR |= GPIO_BSRR_BS_15;
		for(uint32_t x = 0; x < 25000; x++){}
		GPIOD->BSRR |= GPIO_BSRR_BR_15;

		uint32_t ticks = pdMS_TO_TICKS(1000);

		vTaskDelay(ticks);
	}

	vTaskDelete(NULL);
}


//RTOS Hooks 
// This function is called every tick interrupt.
void vApplicationTickHook(void)
{
    // Optional: Add your custom tick hook code here
	printf("Application Tick Hook has been Run \n");
}

// This function is called when a stack overflow is detected.
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // Optional: Add your custom stack overflow handling code here
    // For example, you can reset the system or enter an infinite loop
	printf("Stack overflow! \n");
    (void)xTask;
    (void)pcTaskName;
    for (;;);
}

void vApplicationMallocFailedHook(void) {
    // Handle malloc failure, e.g., log an error or reset the system
    // For now, an empty implementation will suffice
	printf("Malloc Failed \n");
    while (1);
}

void vApplicationIdleHook(void) {
    // Perform low-priority tasks or go to low-power mode
    // For now, an empty implementation will suffice
	printf("We are in Idle Hook \n");
}


void I2C1_EV_IRQHandler (void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler (void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle,uint8_t AppEv)
{
     if(AppEv == I2C_EV_TX_CMPLT)
     {
    	 printf("i2c Tx is completed\n");
     }else if (AppEv == I2C_EV_RX_CMPLT)
     {
    	 printf("Rx is completed\n");
    	 //rxComplt = SET;
     }else if (AppEv == I2C_ERROR_AF)
     {
    	 printf("Error : Ack failure\n");
    	 //in master ack failure happens when slave fails to send ack for the byte
    	 //sent from the master.
    	 I2C_CloseSendData(pI2CHandle);

    	 //generate the stop condition to release the bus
    	 I2C_GenerateStopCondition(I2C1);

    	 //Hang in infinite loop
    	 //while(1);
     }
}

