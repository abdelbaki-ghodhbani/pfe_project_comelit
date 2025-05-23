#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"
#include "hardware_module.h"

#define SSID_NAME  "Cred_Intern"
#define PASSKEY    "cred@2024+"

int main()
{


   /*Initialize debug uart*/
   debug_uart_init();
   esp_uart_init();
   /*Initialize hardware module*/
   light_int();

   /*Initialize circular buffer*/
   circular_buffer_init();
   /*esp configuration functions*/
	ESP_Config esp_config;
	    strcpy(esp_config.ssid, SSID_NAME);
	    strcpy(esp_config.password, PASSKEY);
	    esp_config.mode = 1; // Set mode to 1 for station mode, adjust as needed
	    /*Initialize server*/

	    esp82xx_sever_init(&esp_config);

while(1)
	{
	/*Activate server*/
	server_begin();
	}

}

