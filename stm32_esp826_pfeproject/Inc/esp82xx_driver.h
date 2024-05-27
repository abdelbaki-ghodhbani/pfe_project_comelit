
#ifndef ESP82XX_DRIVER_H_
#define ESP82XX_DRIVER_H_
#include <stdint.h>
#include "stm32f4xx.h"

#define SR_TXE			(1U<<7)
#define SR_RXNE			(1U<<5)
// Initialize the debug UART for communication
void debug_uart_init(void);
// Write a character to the debug UART
void debug_uart_write(int ch);
// Initialize the ESP UART for communication
void esp_uart_init(void);
// generate delay in milliseconds using SysTick timer
void systick_delay_ms(uint32_t delay);


#endif /* ESP82XX_DRIVER_H_ */
