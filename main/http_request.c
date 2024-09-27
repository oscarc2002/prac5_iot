#include "http_request.h"

char ip_local[30] = "";

void send_http_request(char ip[])
{
    esp_http_client_config_t config = {
        .url = ip,
        .event_handler = http_event_handler_ip_request,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    esp_http_client_cleanup(client);
}

void send_http_request_db(char ip[], char username[], char password[])
{
    esp_http_client_config_t config = {
        .url = ip,
        .event_handler = http_event_handler_ip_request,
        .username = username,
        .password = password,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_perform(client);

    esp_http_client_cleanup(client);
}

esp_err_t http_event_handler_ip_request(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI("HTTP CLIENT", "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI("HTTP CLIENT", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        //ESP_LOGI("HTTP CLIENT", "Received data: %.*s\n", evt->data_len, (char *)evt->data);
        getIP(evt->data);

        break;
    default:
        break;
    }
    return ESP_OK;
}

void getIP(char string[])
{
    int i = 0;
    int len = 13;
    while (i < len)
    {
        ip_local[i] = string[15 + i];
        i++;
    }

    ip_local[i] = '\0';
}