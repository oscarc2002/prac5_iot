#include "esp_log.h"
#include "lwip/sockets.h"
#include "tcp_packet.h"

#define LED 32
#define HIGH 1
#define LOW 0

extern int addr_family;
extern int ip_protocol;

int create_UDP_socket(int port);
char setLedStateUDP(int sock, char state, struct sockaddr_storage source_addr);
void sendLedStateUDP(int sock, char ledState[2], struct sockaddr_storage source_addr);
int sendADCStateUDP(int sock, struct sockaddr_storage source_addr);