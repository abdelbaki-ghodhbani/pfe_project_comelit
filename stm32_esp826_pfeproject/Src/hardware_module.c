#include "hardware_module.h"
#define GPIOA_EN   (1U<<0)

// function to initialize the hardware module for controlling the light
	void light_int(void){
	//enable clock access to the PA port
	RCC->AHB2ENR  |=GPIOA_EN;
	// set pin pa5 to output
	GPIOA -> MODER |=  (1U<<10);
	GPIOA -> MODER &=~(1U<<11);
	}
	// function to Turn on the light

	void light_on(void)
	{
		/*Set PA5 output to high*/
		GPIOA->ODR |=(1U<<5);
	}
	// function to Turn off the light

	void light_off(void)
	{
		/*Set PA5 output to LOW*/
		GPIOA->ODR &=~(1U<<5);
	}




