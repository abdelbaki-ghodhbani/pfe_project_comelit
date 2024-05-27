#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_

#include <stdint.h>

/* Structure to hold the configuration parameters for the ESP82xx module */
typedef struct {
    char ssid[48];       // SSID of the Wi-Fi network
    char password[48];   // Password of the Wi-Fi network
    int mode;            // Operating mode of the ESP82xx (e.g., station, softAP, both)
} ESP_Config;

/* Function to initialize the ESP82xx server with the given configuration */
void esp82xx_sever_init(ESP_Config *config);

/* Function to handle server requests based on the URL path */
void Server_Handle(char *str, int Link_ID);

/* Function to start the server, listen for incoming requests, and handle them */
void server_begin(void);

/* UART interrupt handler for USART1 (handles ESP82xx UART callbacks) */
void USART1_IRQHandler(void);

/* UART interrupt handler for USART2 (handles debug UART callbacks) */
void USART2_IRQHandler(void);

#endif /* ESP82XX_LIB_H_ */

