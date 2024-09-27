#include "esp_log.h"
#include "esp_http_client.h"

extern char ip_local[30];

void send_http_request(char ip[]);
void send_http_request_db(char ip[], char username[], char password[]);
esp_err_t http_event_handler_ip_request(esp_http_client_event_t *evt);
void getIP(char string[]);