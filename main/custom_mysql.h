#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "lwip/sockets.h"

int connect_to_mysql(const char* ip, int port);
void send_query(int sock, const char* query);
void send_data(int sock, const char* device, const char* ip, const char* rssi, const char* led, const char* adc) ;