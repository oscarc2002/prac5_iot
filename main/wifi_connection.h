#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"

#define SSID "IZZI-C6EB"
#define PASSWORD "9CC8FC72C6EB"

static void wifi_sta_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_connection();
