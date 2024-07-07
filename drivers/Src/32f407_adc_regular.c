#include <stdio.h>

#include "../Inc/32f407_adc_regular.h"
#include "../Inc/32f407_delay_timer.h"




int8_t AdcInit(){


	// Configure ADC so we accept one in input on one ch
	//enable ADC1 clock
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	//choose 12 bit resolution
	ADC1->CR1 &= ~(3 << ADC_CR1_RES_Pos);
	// conversion mode, single default
	ADC1->CR2 &= ~(1  << ADC_CR2_CONT_Pos);

	// set channel to be read ADC1_IN16
	ADC1->SQR3 &= ~(0x1F);
	uint32_t channel_number = 16;
	ADC1->SQR3 |= (channel_number  & 0x1F);

	// Sample Rate
	ADC1->SMPR1 |= (7 << ADC_SMPR1_SMP16_Pos);

	//Enable temp sensor
	ADC->CCR |= (1 << ADC_CCR_TSVREFE_Pos);

	//Start ADC1
	ADC1->CR2 |= (1 << ADC_CR2_ADON_Pos);


}

uint16_t AdcReadChannel(uint8_t  ChannelNo){
    // Read input and print it
	ADC1->CR2  |= (1 << ADC_CR2_SWSTART_Pos);

	// wait for end of conversion
	while(!(ADC1->SR & (1 << ADC_SR_EOC_Pos)));

	//read adc_raw_input
	uint16_t temperature_raw = ADC1->DR;

	
	float Vsense = (float)temperature_raw/4096 * 3.3;

	printf("Raw value to Voltage from ADC Conversion %3.3f \n", Vsense);

	float VDeg =((Vsense - 0.76)/0.25) + 25;

    printf("Temp after applying Vsense Conversion: %2.3fC \n", VDeg);


    return temperature_raw;
}
