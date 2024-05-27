#include <stdio.h>
#include <string.h>
#include "esp82xx_driver.h"
#include "circular_buffer.h"
#include "esp82xx_lib.h"
#include "hardware_module.h"
// Function prototypes for internal static functions
static void esp82xx_reset(void);
static void esp82xx_startup_test(void);
static void esp82xx_sta_mode(ESP_Config *config);
static void esp82xx_ap_connect(ESP_Config *config);
static void esp82xx_get_local_ip(void);
static void esp82xx_multi_conn_en(void);
static void esp82xx_create_tcp_server(void);
static int8_t send_server_data(char *str, int Link_ID);
// UART port types

extern portType esp82xx_port;
extern portType debug_port;
//html code for led status is off

char* home= "<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"
		"<title>LED CONTROL</title>\n"
		"<style>\n"
		"html, body {\n"
		"    height: 100%;\n"
		"    margin: 0;\n"
		"    padding: 0;\n"
		"    background-color: #1a1a1a;\n"
		"    color: #ffffff;\n"
		"    font-family: Helvetica;\n"
		"    display: flex;\n"
		"    flex-direction: column;\n"
		"    justify-content: center;\n"
		"    align-items: center;\n"
		"}\n"
		"\n"
		"h1 {\n"
		"    margin: 20px;\n"
		"    font-size: 24px;\n"
		"}\n"
		"\n"
		".container {\n"
		"    width: 100%;\n"
		"    max-width: 600px;\n"
		"    padding: 20px;\n"
		"    margin-top: 20px;\n"
		"    background-color: #282828;\n"
		"    border-radius: 10px;\n"
		"    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
		"    text-align: center;\n"
		"}\n"
		"\n"
		"p {\n"
		"    font-size: 18px;\n"
		"    margin-bottom: 20px;\n"
		"}\n"
		"\n"
		".button {\n"
		"    display: inline-block;\n"
		"    width: 120px;\n"
		"    padding: 15px;\n"
		"    background-color: #4CAF50;\n"
		"    border: none;\n"
		"    color: #ffffff;\n"
		"    font-size: 18px;\n"
		"    text-align: center;\n"
		"    text-decoration: none;\n"
		"    margin: 0 auto;\n"
		"    border-radius: 5px;\n"
		"    transition: background-color 0.3s ease;\n"
		"}\n"
		"\n"
		".button:hover {\n"
		"    background-color: #45a049;\n"
		"}\n"
		"\n"
		".lamp-off {\n"
		"    font-size: 5em;\n"
		"    color: #555;\n"
		"    filter: brightness(0.5) grayscale(100%);\n"
		"    margin-top: 20px;\n"
		"}\n"
		"</style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>HARDWARE CONTROL SERVER</h1>\n"
		"    <div class=\"container\">\n"
		"        <p>LED Status: OFF</p>\n"
		"        <div class=\"button-container\">\n"
		"            <a class=\"button\" href=\"/ledon\">Turn ON</a>\n"
		"        </div>\n"
		"        <div class=\"lamp-off\">&#x1F4A1;</div>\n"
		"    </div>\n"
		"</body>\n"
		"</html>";


//html code for led status is  on
char* light= "<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"
		"<title>LED CONTROL</title>\n"
		"<style>\n"
		"html, body {\n"
		"    height: 100%;\n"
		"    margin: 0;\n"
		"    padding: 0;\n"
		"    background-color: #1a1a1a;\n"
		"    color: #ffffff;\n"
		"    font-family: Helvetica;\n"
		"    display: flex;\n"
		"    flex-direction: column;\n"
		"    justify-content: center;\n"
		"    align-items: center;\n"
		"}\n"
		"\n"
		"h1 {\n"
		"    margin: 20px;\n"
		"    font-size: 24px;\n"
		"}\n"
		"\n"
		".container {\n"
		"    width: 100%;\n"
		"    max-width: 600px;\n"
		"    padding: 20px;\n"
		"    margin-top: 20px;\n"
		"    background-color: #282828;\n"
		"    border-radius: 10px;\n"
		"    box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);\n"
		"    text-align: center;\n"
		"}\n"
		"\n"
		"p {\n"
		"    font-size: 18px;\n"
		"    margin-bottom: 20px;\n"
		"}\n"
		"\n"
		".button {\n"
		"    display: inline-block;\n"
		"    width: 120px;\n"
		"    padding: 15px;\n"
		"    background-color: #FF5733; /* red */\n"
		"    border: none;\n"
		"    color: #ffffff;\n"
		"    font-size: 18px;\n"
		"    text-align: center;\n"
		"    text-decoration: none;\n"
		"    margin: 0 auto;\n"
		"    border-radius: 5px;\n"
		"    transition: background-color 0.3s ease;\n"
		"}\n"
		"\n"
		".button:hover {\n"
		"    background-color: #ff2200; /* darker red on hover */\n"
		"}\n"
		"\n"
		".lamp-on {\n"
		"    font-size: 5em;\n"
		"    color: yellow;\n"
		"    margin-top: 20px;\n"
		"}\n"
		"</style>\n"
		"</head>\n"
		"<body>\n"
		"    <h1>HARDWARE CONTROL SERVER</h1>\n"
		"    <div class=\"container\">\n"
		"        <p>LED Status: ON</p>\n"
		"        <a class=\"button\" href=\"/ledoff\">Turn OFF</a>\n"
		"        <div class=\"lamp-on\">&#x1F4A1;</div>\n"
		"    </div>\n"
		"</body>\n"
		"</html>\n";



/* Initializes the ESP82xx server with the given configuration */
void esp82xx_sever_init(ESP_Config *config) {
    /* Initialize the circular buffer for UART communication */
    circular_buffer_init();

    /* Reset the ESP82xx module */
    esp82xx_reset();

    /* Perform startup test to ensure ESP82xx is responding */
    esp82xx_startup_test();

    /* Configure the ESP82xx module to station mode with the provided configuration */
    esp82xx_sta_mode(config);

    /* Connect the ESP82xx module to the Access Point (AP) with the provided configuration */
    esp82xx_ap_connect(config);

    /* Retrieve and display the local IP address of the ESP82xx module */
    esp82xx_get_local_ip();

    /* Enable multiple connections on the ESP82xx module */
    esp82xx_multi_conn_en();

    /* Create a TCP server on the ESP82xx module */
    esp82xx_create_tcp_server();
}


/*Reset esp82xx*/

 static void esp82xx_reset(void){
	/*Send reset command*/
	buffer_send_string("AT+RST\r\n",esp82xx_port);

	/*Delay for 1 second*/
	systick_delay_ms(1000);

	/*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("Reset was successful...\n\r",debug_port);
}

 /*Test AT startup*/
 static void esp82xx_startup_test(void){
	 /*Clear esp uart buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send test command*/
	buffer_send_string("AT\r\n",esp82xx_port);



     /*Wait for "OK" response*/
	while(! (is_response("OK\r\n"))){}

	buffer_send_string("AT Startup test successful...\n\r",debug_port);

}

 /*configure station mode*/
  static void esp82xx_sta_mode(ESP_Config *config)
  {
	  char command[20];

	      /* Clear esp uart buffer */
	      buffer_clear(esp82xx_port);

	      /* Create command based on mode */
	      sprintf(command, "AT+CWMODE=%d\r\n", config->mode);

	      /* Send mode command */
	      buffer_send_string(command, esp82xx_port);

     /*Wait for "OK" response*/
 	while(! (is_response("OK\r\n"))){}

 	/* Provide feedback based on mode */
 	    if (config->mode == 1) {
 	        buffer_send_string("Mode station set successfully...\n\r", debug_port);
 	    } else if (config->mode == 2) {
 	        buffer_send_string("SoftAP mode set successfully...\n\r", debug_port);
 	    } else if (config->mode == 3) {
 	        buffer_send_string("SoftAP+Station mode set successfully...\n\r", debug_port);
 	    } else {
 	        buffer_send_string("Unknown mode...\n\r", debug_port);
 	    }
 	}



  /*Connect to Access Apoint (AP)*/

  static void esp82xx_ap_connect(ESP_Config *config)
   {
  	 char data[80];

  	 /*Clear esp uart buffer*/
  	 buffer_clear(esp82xx_port);

  	 buffer_send_string("Connecting to access point...\n\r",debug_port);

  	 /*Put ssid,password  and command into one packet*/
  	 sprintf(data,"AT+CWJAP=\"%s\",\"%s\"\r\n" ,config->ssid, config->password);

  	 /*Send entire packet to esp uart*/
  	 buffer_send_string(data,esp82xx_port);

  	  /*Wait for "OK" response*/
  	 while(! (is_response("OK\r\n"))){}

  	 sprintf(data, "Connected : \"%s\"\r\n",config->ssid);

  	 buffer_send_string(data,debug_port);

   }

 /*Get device local IP address*/

  static void esp82xx_get_local_ip(void)
 {
	 char data[80];
	 char buffer[20];
	 int len;

	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'get ip address ' command*/
	 buffer_send_string("AT+CIFSR\r\n",esp82xx_port);

	 /*Wait for 'Station IP'  response*/
	 while(!(is_response("CIFSR:STAIP,\""))){}

	 /*Copy IP address portion*/
	 while(!(copy_up_to_string("\"",buffer))){}

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 len = strlen(buffer);

	 buffer[len - 1] = '\0';

	 sprintf(data,"Local IP Address :  %s \r\n",buffer);

	 buffer_send_string(data,debug_port);


 }

 /*Enable multiple connections*/
  static void esp82xx_multi_conn_en(void)
 {
	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'enable multi conn cmd'*/
	 buffer_send_string("AT+CIPMUX=1\r\n",esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 buffer_send_string("Multiple connections enabled....\n\r",debug_port);

 }


 /*Creat tcp server*/

  static void esp82xx_create_tcp_server(void)
 {
	 /*Clear buffer*/
	 buffer_clear(esp82xx_port);

	 /*Send 'create tcp server cmd'*/
	 buffer_send_string("AT+CIPSERVER=1,80\r\n",esp82xx_port);

	  /*Wait for "OK" response*/
	 while(! (is_response("OK\r\n"))){}

	 buffer_send_string("Please connect to the Local IP above....\n\r",debug_port);

 }


 /*Send data to the server*/

 static int8_t send_server_data(char *str , int Link_ID)
  {
 	 char data[80];

 	 /*Get length of data to send*/
 	 int len = strlen(str);

 	 /*Send link id and data length*/
 	 sprintf(data,"AT+CIPSEND=%d,%d\r\n",Link_ID,len);
 	 buffer_send_string(data,esp82xx_port);

 	  /*Wait for ">" response*/
 	  while(! (is_response(">"))){}

 	  /*Send data*/
 	  buffer_send_string(str,esp82xx_port);

 	  /*Wait for "SEND OK" response*/
 	   while(! (is_response("SEND OK"))){}

 	  /*Close connection*/
 	   sprintf(data,"AT+CIPCLOSE=5\r\n");
 	   buffer_send_string(data,esp82xx_port);

 		/*Wait for "OK" response*/
 	   while(! (is_response("OK\r\n"))){}

 	   return 1;

  }
 /* Handles the server request based on the URL path */
 void Server_Handle(char *str, int Link_ID)
 {
     /* If the requested URL is "/ledon" */
     if (!(strcmp(str, "/ledon")))
     {
         /* Send the HTML content for the LED ON status */
         send_server_data(light, Link_ID);
     }
     /* If the requested URL is "/ledoff" */
     else if (!(strcmp(str, "/ledoff")))
     {
         /* Send the HTML content for the LED OFF status */
         send_server_data(home, Link_ID);
     }
     /* For any other URL, default to LED OFF status */
     else
     {
         /* Send the HTML content for the LED OFF status */
         send_server_data(home, Link_ID);
     }
 }

 /* Begins the server, listens for incoming requests and handles them */
 void server_begin(void)
 {
     char dest_buffer[64] = {0};  // Buffer to store the request data
     char link_id;                // Variable to store the Link ID

     /* Wait for an incoming connection with "+IPD," prefix */
     while (!(get_next_strs("+IPD,", 1, &link_id))) {}

     link_id -= 48;  // Convert link_id from ASCII to integer

     /* Wait until "HTTP/1.1" is found in the request */
     while (!(copy_up_to_string("HTTP/1.1", dest_buffer))) {}

     /* Check if the request is for "/ledon" */
     if (find_str("/ledon", dest_buffer) == 1)
     {
         /* Handle the "/ledon" request */
         Server_Handle("/ledon", link_id);

         /* Turn the LED on */
         light_on();

         /* Send debug message */
         buffer_send_string("\r\nLight module is on..\r\n", debug_port);
     }
     /* Check if the request is for "/ledoff" */
     else if (find_str("/ledoff", dest_buffer) == 1)
     {
         /* Handle the "/ledoff" request */
         Server_Handle("/ledoff", link_id);

         /* Turn the LED off */
         light_off();

         /* Send debug message */
         buffer_send_string("\r\nLight module is off...\r\n", debug_port);
     }
     /* Check if the request is for "/favicon.ico" (typically ignored) */
     else if (find_str("/favicon.ico", dest_buffer) == 1)
     {
         // Ignore favicon requests, often made automatically by browsers
     }
     /* Handle any other request */
     else
     {
         /* Turn the LED off */
         light_off();

         /* Handle the default request */
         Server_Handle("/", link_id);
     }
 }

 /* UART interrupt handler for USART1 */
 void USART1_IRQHandler(void)
 {
     /* Handle the UART callback for ESP82xx */
     esp82_uart_callback();
 }

 /* UART interrupt handler for USART2 */
 void USART2_IRQHandler(void)
 {
     /* Handle the UART callback for debug */
     debug_uart_callback();
 }












