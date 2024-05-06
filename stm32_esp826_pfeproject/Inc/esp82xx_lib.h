#ifndef ESP82XX_LIB_H_
#define ESP82XX_LIB_H_
#include <stdint.h>

typedef struct {
    char ssid[48];
    char password[48];
    int mode;

} ESP_Config;

void esp82xx_sever_init(ESP_Config *config);
void Server_Handle (char *str, int Link_ID);
void server_begin(void);
void USART1_IRQHandler(void);
void USART2_IRQHandler(void);



#endif /* ESP82XX_LIB_H_ */
