#include <string.h>
#include "circular_buffer.h"
// UART control and status register bit definitions

#define CR1_RXNEIE		(1U<<5)
#define CR1_TXEIE		(1U<<7)
#define SR_RXNE		(1U<<5)
#define SR_TXE		(1U<<7)
// Function prototypes for internal static functions
static void buff_store_char(unsigned char c, circular_buffer * buffer);
static void get_first_char(char *str);
static int8_t process_copy(char * str, char * dest_buffer, int start_pos);
// UART port types

portType debug_port 	= 	0;
portType esp82xx_port 	=  	1;

/*Buffers for ESP_UART*/
circular_buffer rx_buffer1 = {{INIT_VAL},INIT_VAL,INIT_VAL};
circular_buffer tx_buffer1 = {{INIT_VAL},INIT_VAL,INIT_VAL};

/*Buffers for DEBUG_UART*/
circular_buffer rx_buffer2 = {{INIT_VAL},INIT_VAL,INIT_VAL};
circular_buffer tx_buffer2 = {{INIT_VAL},INIT_VAL,INIT_VAL};

circular_buffer *_rx_buffer1;
circular_buffer *_tx_buffer1;
circular_buffer *_rx_buffer2;
circular_buffer *_tx_buffer2;

// Initialize circular buffers and UART interrupts
void circular_buffer_init(void)
{
	/*Initial buffers*/
	_rx_buffer1 = &rx_buffer1;
	_tx_buffer1 = &tx_buffer1;
	_rx_buffer2 = &rx_buffer2;
	_tx_buffer2 = &tx_buffer2;

	/*Initial UART interrupts*/
	USART1->CR1 |=CR1_RXNEIE;  				/*esp_uart*/
	USART2->CR1 |=CR1_RXNEIE;  				/*debug_uart*/


}

// function to store a character in the circular buffer
static void buff_store_char(unsigned char c, circular_buffer * buffer)
{
	 int loc =  (uint32_t)(buffer->head +1)% UART_BUFFER_SIZE;

	/*Check if no overflow will occur*/

	 if(loc != buffer->tail){
		 /*Store character*/
		 buffer->buffer[buffer->head]  = c ;
		 /*Update head*/
		 buffer->head = loc;
	 }
}

// function to find the position of str2 in str1
int8_t find_str(char *str2, char *str1)
{
  int l, i, j;
  /*Count substring i.r str2*/
  for (l = 0; str2[l] != '\0'; l++){}

   for (i = 0, j = 0; str1[i] != '\0' && str2[j] != '\0'; i++)
   {
       if (str1[i] == str2[j])
       {
           j++;
       }
       else
       {
           j = 0;
       }
   }

   if (j == l)
   {
	   /*success*/
       return 1;
   }
   else
   {
       return -1;

   }

}

// function to clear the circular buffer for the specified UART port
void buffer_clear(portType uart)
{
	if(uart == esp82xx_port )
	{
		/*Set buffer content to  '\0'*/
		memset(_rx_buffer1->buffer,'\0',UART_BUFFER_SIZE);
		_rx_buffer1->head = 0 ;
	}

	if(uart == debug_port )
	{
		/*Set buffer content to  '\0'*/
		memset(_rx_buffer2->buffer,'\0',UART_BUFFER_SIZE);
		_rx_buffer2->head = 0 ;
	}
}

//function to see the content of the circular buffer
int32_t buffer_peek(portType uart)
{

    if (uart == esp82xx_port)
    {
        if (_rx_buffer1->head == _rx_buffer1->tail)
        {   // Indicate buffer is empty
            return -1;
        }
        else
        {   // Return the next character in the buffer
            return _rx_buffer1->buffer[_rx_buffer1->tail];
        }
    }

    else if (uart == debug_port)
    {

        if (_rx_buffer2->head == _rx_buffer2->tail)
        {   // Indicate buffer is empty
            return -1;
        }
        else
        {   // Return the next character in the buffer
            return _rx_buffer2->buffer[_rx_buffer2->tail];
        }
    }
    else
    {  // Fail
        return -1;
    }
}
//function to read the content of the buffer and put it in char
int32_t buffer_read(portType uart)
{

    if (uart == esp82xx_port)
    {

        if (_rx_buffer1->head == _rx_buffer1->tail)
        {   // Indicate buffer is empty
            return -1;
        }
        else
        {
            unsigned char c = _rx_buffer1->buffer[_rx_buffer1->tail];
            _rx_buffer1->tail = (uint32_t)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
            // Return the character read
            return c;
        }
    }

    else if (uart == debug_port)
    {

        if (_rx_buffer2->head == _rx_buffer2->tail)
        {  // Indicate buffer is empty
            return -1;
        }
        else
        {
            unsigned char c = _rx_buffer2->buffer[_rx_buffer2->tail];
            _rx_buffer2->tail = (uint32_t)(_rx_buffer2->tail + 1) % UART_BUFFER_SIZE;
            // Return the character read
            return c;
        }
    }
    else
    {  // Fail
        return -1;
    }
}


//function to write a character to the buffer
void buffer_write(unsigned char c, portType uart)
{

    if (uart == esp82xx_port)
    {   // Calculate the next position in the buffer
        int loc = (_tx_buffer1->head + 1) % UART_BUFFER_SIZE;

        // Wait until there is space available in the buffer
        while (loc == _tx_buffer1->tail) {}

        _tx_buffer1->buffer[_tx_buffer1->head] = c;
        _tx_buffer1->head = loc;

        // Enable UART Transmit interrupt
        USART1->CR1 |= CR1_TXEIE;
    }

    else if (uart == debug_port)
    {   // Calculate the next position in the buffer
        int loc = (_tx_buffer2->head + 1) % UART_BUFFER_SIZE;

        // Wait until there is space available in the buffer
        while (loc == _tx_buffer2->tail) {}

        _tx_buffer2->buffer[_tx_buffer2->head] = c;
        _tx_buffer2->head = loc;

        // Enable UART Transmit interrupt
        USART2->CR1 |= CR1_TXEIE;
    }
}

// function to check if there is data available in the buffer
int32_t is_data(portType uart)
{

    if (uart == esp82xx_port)
    {

        return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer1->head - _rx_buffer1->tail) % UART_BUFFER_SIZE;
    }

    else if (uart == debug_port)
    {

        return (uint16_t)(UART_BUFFER_SIZE + _rx_buffer2->head - _rx_buffer2->tail) % UART_BUFFER_SIZE;
    }
    else
    {
        // Fail
        return -1;
    }
}



/*Function to find the position of the first character of a string in the
 * the buffer*/


static void get_first_char(char *str)
{
	/*Make sure there is data in the buffer*/
	while(!is_data(esp82xx_port)){}

	while(buffer_peek(esp82xx_port)!= str[0]){

		_rx_buffer1->tail =  (uint16_t)(_rx_buffer1->tail +1)%UART_BUFFER_SIZE;

		while(!is_data(esp82xx_port)){}

	}
}

//function to find response within our our received packets
int8_t is_response(char *str)
{
	   int curr_pos = 0; // Current position in the response string
	    int len = strlen(str); // Length of the response strin

	while( curr_pos != len)
	{
		curr_pos  = 0;
		get_first_char(str);

		while(buffer_peek(esp82xx_port) == str[curr_pos])
		{
			curr_pos++;

			buffer_read(esp82xx_port);

			if(curr_pos ==  len)
			{
				/*success*/
				return 1;
			}

			while(!is_data(esp82xx_port)){}

		}

	}

	if(curr_pos  == len)
	{
		/*success*/
	   return 1;
	}
	else
	{  /*fail*/
	   return -1;
	}
}

// function to get the next characters from the buffer and store it

int8_t get_next_strs(char *str,uint8_t num_of_chars, char *dest_buffer)
{
	/*Make sure the string exists*/
	while(!is_response(str)){}

	for(int indx=0; indx < num_of_chars ; indx++)
	{
		while(!is_data(esp82xx_port)){}
		dest_buffer[indx] =  buffer_read(esp82xx_port);

	}
	/*success*/
	return 1;
}

//function to send a string

void buffer_send_string(const char *s, portType uart)
{
	while(*s != '\0')
	{
		buffer_write(*s++,uart);
	}
}
//function  to extract characters from the receive buffer until encountering the specified strin
static int8_t process_copy(char * str, char * dest_buffer, int start_pos)
{
	int curr_pos = 0;
	int len =  strlen(str);
	int indx = start_pos;
    // Wait for data to be available in the buffer
	while(!is_data(esp82xx_port)){}

	while(buffer_peek(esp82xx_port) != str[curr_pos] )
	{
		dest_buffer[indx] =  _rx_buffer1->buffer[_rx_buffer1->tail];
		_rx_buffer1->tail =  (uint16_t)(_rx_buffer1->tail + 1) % UART_BUFFER_SIZE;
		indx++;
		// Wait for more data to be available in the buffer
		while(!is_data(esp82xx_port)){}


	}

	while(buffer_peek(esp82xx_port) == str[curr_pos])
	{
		curr_pos++;
		dest_buffer[indx++] = buffer_read(esp82xx_port);

		if(curr_pos == len){
			/*success*/
			return 1;
		}

		while(!is_data(esp82xx_port)){}

	}

	if(curr_pos != len)
	{
		curr_pos = 0;
		// Recursive call to continue searching for the string

		process_copy(str, dest_buffer, indx);

	}

	if(curr_pos  ==  len)
	{  /*success*/
		return 1;
	}
	else
	{  /*fail*/
		return -1;
	}



}
// function to copy characters from the buffer until encountering the specified string
int8_t copy_up_to_string(char * str, char * dest_buffer)
{
	/*Process copy*/
	return (process_copy(str,dest_buffer, 0));
}
// UART callback function for the ESP82XX port  interrupts

void esp82_uart_callback(void)
{
	/*Check if RXNE is raised and also if RXNEIE is enabled*/
	if(((USART1->SR & SR_RXNE) != 0) && ((USART1->CR1 & CR1_RXNEIE) != 0))
	{
		unsigned char c = USART1->DR;
		buff_store_char(c,_rx_buffer1);

	}

	/*Check if TXE is raised and also if TXEIE is enabled*/

	if(((USART1->SR & SR_TXE) != 0) && ((USART1->CR1 & CR1_TXEIE) != 0))
	{
		if(tx_buffer1.head  == tx_buffer1.tail)
		{
			USART1->CR1 &=~CR1_TXEIE;
		}
		else
		{
			unsigned char c  = tx_buffer1.buffer[tx_buffer1.tail];
			tx_buffer1.tail =  (tx_buffer1.tail + 1) % UART_BUFFER_SIZE;
			USART1->DR = c;

		}
	}

}
// UART callback function for the debug port  interrupts

void debug_uart_callback(void)
{
	/*Check if RXNE is raised and also if RXNEIE is enabled*/
	if(((USART2->SR & SR_RXNE) != 0) && ((USART2->CR1 & CR1_RXNEIE) != 0))
	{
		unsigned char c = USART2->DR;
		buff_store_char(c,_rx_buffer2);

	}

	/*Check if TXE is raised and also if TXEIE is enabled*/

	if(((USART2->SR & SR_TXE) != 0) && ((USART2->CR1 & CR1_TXEIE) != 0))
	{
		if(tx_buffer2.head  == tx_buffer2.tail)
		{
			USART2->CR1 &=~CR1_TXEIE;
		}
		else
		{
			unsigned char c  = tx_buffer2.buffer[tx_buffer2.tail];
			tx_buffer2.tail =  (tx_buffer2.tail + 1) % UART_BUFFER_SIZE;
			USART2->DR = c;

		}
	}
}
