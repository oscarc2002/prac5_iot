#include "wifi_connection.h"

static void wifi_sta_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id)
    {
    case WIFI_EVENT_STA_START:
        esp_wifi_connect();
        ESP_LOGI("WIFI STATUS", "Conectado a red wifi\n");
        break;
    case WIFI_EVENT_STA_CONNECTED:
        ESP_LOGI("WIFI STATUS", "Estación conectada");
        break;
    case WIFI_EVENT_STA_DISCONNECTED:
        ESP_LOGE("WIFI STATUS", "Conexión fallida. Intentando reconectar...");
        break;
    case IP_EVENT_STA_GOT_IP:
        ESP_LOGI("WIFI STATUS", "WiFi obtuvo la IP ... \n\n");
        break;
    default:
        break;
    }
}

void wifi_connection()
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();

    esp_netif_create_default_wifi_sta();
    esp_wifi_init(&wifi_initiation);
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_sta_event_handler, NULL);
    wifi_config_t wifi_configuration = {
        .sta = {
            .ssid = SSID,
            .password = PASSWORD,
        },
    };

    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    esp_wifi_connect();
}