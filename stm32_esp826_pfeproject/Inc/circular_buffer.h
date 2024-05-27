#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#include "stm32f4xx.h"
#include <stdint.h>

#define UART_BUFFER_SIZE		512  // Maximum size of the circular buffer for UART data
#define INIT_VAL				0   // Initial value used for buffer initialization
typedef uint8_t portType;// Type definition for UART port identifier
// Structure representing a circular buffer
typedef struct
{
	unsigned char buffer[UART_BUFFER_SIZE]; // Circular buffer array to store data
	volatile uint32_t head;                 //head pointer of the circular buffer for writing
	volatile uint32_t tail;                 //tail pointer of the circular buffer for reading

}circular_buffer;
// Initialize circular buffers and UART interrupts
void circular_buffer_init(void);
// function to clear the circular buffer for the specified UART port
void buffer_clear(portType uart);
// function to find the position of str2 in str1
int8_t find_str(char *str2, char *str1);
//function to see the content of the circular buffer
int32_t buffer_peek(portType uart);
//function to read the content of the buffer and put it in char
int32_t buffer_read(portType uart);
//function to write a character to the buffer
void buffer_write(unsigned char c, portType uart );
// function to check if there is data available in the buffer
int32_t is_data(portType uart);
//function to find response within our our received packets
int8_t is_response(char *str);
// function to get the next characters from the buffer and store it
int8_t get_next_strs(char *str,uint8_t num_of_chars, char *dest_buffer);
//function to send a string
void buffer_send_string(const char *s, portType uart);
// function to copy characters from the buffer until encountering the specified string
int8_t copy_up_to_string(char * str, char * dest_buffer);
// UART callback function for the ESP82XX port  interrupts
void esp82_uart_callback(void);
// UART callback function for the debug port  interrupts
void debug_uart_callback(void);


#endif /* CIRCULAR_BUFFER_H_ */
