#include <stdio.h>
#include "esp_log.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#define VCC 13
#define LED 32
#define HIGH 1
#define LOW 0

extern char ack[16];

//void sendTCP(int sockServer, char command[128]);
esp_err_t configGPIOS();
void sendNack(int sock);
char setLedState(int sock, char state);
void resetACK();
void sendLedState(int sock, char ledState[2]);
void sendADCState(int sock);

