//? Oscar Arturo Cebrián Reyes 1280124
/* 
    El código son modificaciones de la práctica pasada, solo adaptándolo a enviar comandos SQL a través de TCP
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "wifi_connection.h"
#include "http_request.h"
#include "custom_mysql.h"
#include "udp_packet.h"

#define PORT 777
#define PORT_SERVER 3306

int sockServer = 0, timerID, sockUDP = 0;
char rx_buffer[128];
char buffer_rssi[12];
char buff_adc[12];
char ledState[2] = "0";
TimerHandle_t timerHandler;
static const char *TAG = "MYSQL";

typedef struct
{
    int rssi;
    struct sockaddr_storage source_addr;
} TaskParams;

void sendData(void *pvParams)
{
    while (1)
    {
        TaskParams *params = (TaskParams *)pvParams;
        vTaskDelay(pdMS_TO_TICKS(300000));
        snprintf(buffer_rssi, sizeof(buffer_rssi), "%d", params->rssi);
        snprintf(buff_adc, sizeof(buff_adc), "%d", sendADCStateUDP(sockUDP, params->source_addr));
        send_data(sockUDP, "OAC", ip_local, buffer_rssi, ledState, (const char *)buff_adc);
    }
    vTaskDelete(NULL);
}

void packet_server_task()
{
    // Obtener rssi
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    ESP_LOGW(TAG, "%d\n", ap.rssi);
    int rssi = (int)ap.rssi;

    char addr_str[128];
    char *nack = "NACK";
    char mysql_ip[] = "191.101.13.32";
    uint8_t isTaskCreated = 0;

    while (1)
    {
        sockUDP = create_UDP_socket(PORT);
        // Obtener IP
        send_http_request("http://httpbin.org/ip");
        printf("IP pública: %s\n", ip_local);

        // MYSQL
        int sock = connect_to_mysql(mysql_ip, 3306);

        if (sock >= 0)
        {
            printf("Enviar acceso a la tabla\n");
            char auth_query[256];
            snprintf(auth_query, sizeof(auth_query), "MYSQL_CONNECT('%s', '%s');", "u598733893_iotusr", "iotdb@UabcTj24");
            send_query(sock, auth_query);

            // Seleccionar la base de datos
            printf("Usar base de datos\n");
            send_query(sock, "USE u598733893_iot_db;");

            // snprintf(buffer_rssi, sizeof(buffer_rssi), "%d", rssi);
            // send_data(sock, "OAC", ip_local, buffer_rssi, ledState, "267");

            // close(sock);
        }

        struct sockaddr_storage source_addr;
        socklen_t socklen = sizeof(source_addr);

        char header[5], user[4], comment[64];
        char operation, element, value;

        while (1)
        {
            ESP_LOGI(TAG, "Esperando datos");
            int len = recvfrom(sockUDP, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            if (len > 0)
            {
                // Reiniciando variables
                memset(header, 0, sizeof(header));
                memset(user, 0, sizeof(user));
                memset(comment, 0, sizeof(comment));

                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "%s", rx_buffer);

                // Obtiene la dirección ip del remitente como string
                inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "Se recibió %d bytes de %s:", len, addr_str);

                // Parámetros
                if (!isTaskCreated)
                {
                    TaskParams params;
                    params.rssi = rssi;
                    params.source_addr = source_addr;
                    xTaskCreate(sendData, "TaskSend", 4096, (void *)&params, 5, NULL);
                    isTaskCreated = 1;
                }

                // Separar elementos del comando
                sscanf(rx_buffer, "%4[^:]:%3[^:]:%c:%c:%c:%63[^\n]", header, user, &operation, &element, &value, comment);

                // Primero hacer Login
                if (!strcmp(header, "UABC"))
                {
                    if (!strcmp(user, "OAC"))
                    {
                        // Solo lectura
                        if (operation == 'R')
                        {
                            if (element == 'L')
                            {
                                // Paquete UDP
                                sendLedStateUDP(sockUDP, ledState, source_addr);
                            }
                            else if (element == 'A')
                            {
                                // Paquete UDP
                                sendADCStateUDP(sockUDP, source_addr);
                            }
                            else
                            {
                                // Error
                                sendto(sockUDP, nack, sizeof(nack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
                            }
                        }
                        // Solo escritura
                        else if (operation == 'W')
                        {
                            if (element == 'L')
                            {
                                if (value == '1' || value == '0')
                                {
                                    // Paquete UDP
                                    ledState[0] = setLedStateUDP(sockUDP, value, source_addr);
                                }
                                else
                                {
                                    // Error
                                    sendto(sockUDP, nack, sizeof(nack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
                                }
                            }
                            else
                            {
                                // Error
                                sendto(sockUDP, nack, sizeof(nack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
                            }
                        }
                        else
                        {
                            // Error
                            sendto(sockUDP, nack, sizeof(nack), 0, (struct sUDPockaddr *)&source_addr, sizeof(struct sockaddr_in));
                        }
                    }
                    else
                    {
                        // Error
                        sendto(sockUDP, nack, sizeof(nack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
                    }
                }
                else
                {
                    // Error
                    sendto(sockUDP, nack, sizeof(nack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
                }
            }
            else
            {
                ESP_LOGI(TAG, "No se recibieron datos");
            }
        }
    }
    // Solo se llega aquí en caso de error
    ESP_LOGE(TAG, "Cerrando los sockets y reiniciando...");
    shutdown(sockUDP, 0);
    close(sockUDP);
}

void app_main()
{
    if (!configGPIOS())
    {
        ESP_LOGI("PORTS", "Puertos configurados correctamente");
    }
    wifi_connection();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    while (1)
    {
        packet_server_task();
        vTaskDelay(500);
    }
}