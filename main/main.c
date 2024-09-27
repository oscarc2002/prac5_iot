//? Oscar Arturo Cebrián Reyes 1280124
/* El código de TCP pertenece a SIMS IOT Devices
    Modificaciones: Modificaciones a los HTTP Request, además de la extracción de datos como la IP pública
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "esp_event_loop.h"
#include "esp_system.h"
#include "tcp_packet.h"
#include "wifi_connection.h"
#include "http_request.h"
#include "wrapper.h"

#define PORT 777
#define PORT_SERVER 3306

uint8_t isConected = 0;
int sockServer = 0, timerID, isWaiting = 0;
char rx_buffer[128];
TimerHandle_t timerHandler;
static const char *TAG = "MYSQL";

/*void sendKeepAlive(void *params)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    vTaskDelete(NULL);
}*/

void packet_server_task()
{
    //Obtener rssi
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    ESP_LOGW(TAG,"%d\n", ap.rssi);
    int rssi = (int) ap.rssi;

    int addr_family = AF_INET;
    int ip_protocol = 0;
    char *nack = "NACK";
    char ledState[2] = "0";
    char host_ip[] = "82.180.173.228";
    //char host_ip[] = "191.101.13.32";

    // Conexión MySQL
    MySQL_Connection* mysql_conn = mysql_connection_new(); // Crear una instancia de la conexión

    const char* server = "192.168.1.100"; // Cambia esto por la dirección IP de tu servidor MySQL
    int port = 3306; // Puerto por defecto de MySQL
    const char* user = "your_user"; // Cambia esto por tu usuario
    const char* password = "your_password"; // Cambia esto por tu contraseña
    const char* db = "your_database"; // Cambia esto por tu base de datos, si es necesario

    // Intentar conectarse al servidor MySQL
    if (mysql_connection_connect(mysql_conn, server, port, user, password, db)) {
        printf("Connected to MySQL server.\n");
        // Aquí puedes continuar con otras operaciones
    } else {
        printf("Failed to connect to MySQL server.\n");
    }

    // No olvides cerrar la conexión cuando hayas terminado
    //mysql_connection_close(mysql_conn);
    //mysql_connection_delete(mysql_conn); // Limpiar la instancia

    while (1)
    {
        sockServer = 0;

        struct sockaddr_in server_addr;
        inet_pton(AF_INET, host_ip, &server_addr.sin_addr);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT_SERVER);

        sockServer = socket(addr_family, SOCK_STREAM, ip_protocol); // Socket TCP

        if (sockServer < 0)
        {
            ESP_LOGE(TAG, "Error al crear los sockets: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Sockets creados!");

        // Colocar timeout a UDP
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        setsockopt(sockServer, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

        int err = connect(sockServer, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (err != 0)
        {
            ESP_LOGE(TAG, "El socket TCP no se pudo conectar: errno %d", errno);
            break;
        }
        else
        {
            ESP_LOGI(TAG, "Conectado!");
        }

        // Obtener IP
        send_http_request("http://httpbin.org/ip");
        printf("IP pública: %s\n", ip_local);
        //send_http_request_db(host_ip,"u598733893_iotusr","iotdb@UabcTj24");
        

        while (1)
        {
            ESP_LOGI(TAG, "Llegamos al final");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        // CONECTARSE AL SERVER
        /*char message[] = "UABC:OAC:L:S:LOGIN";
        send(sockServer, message, strlen(message), 0);
        // Respuesta del server
        int len = recv(sockServer, rx_buffer, sizeof(rx_buffer) - 1, 0);
        rx_buffer[len] = 0;
        ESP_LOGI(TAG, "Respuesta del server al login: %s", rx_buffer);

        isConected = 1;
        //xTaskCreate(sendKeepAlive, "TaskSend", 4096, NULL, 5, NULL);

        char header[5], user[4], comment[64];
        char operation, element, value;

        while (isConected)
        {
            ESP_LOGI(TAG, "Esperando datos");
            while (isWaiting)
            {
                vTaskDelay(100);
            }
            isWaiting = 1;
            len = recv(sockServer, rx_buffer, sizeof(rx_buffer) - 1, 0);

            if (len > 0)
            {
                // Reiniciando variables
                memset(header, 0, sizeof(header));
                memset(user, 0, sizeof(user));
                memset(comment, 0, sizeof(comment));

                rx_buffer[len] = 0;
                ESP_LOGI(TAG, "%s", rx_buffer);

                // Separar elementos del comando
                sscanf(rx_buffer, "%4[^:]:%3[^:]:%c:%c:%c:%63[^\n]", header, user, &operation, &element, &value, comment);

                // Primero hacer Login
                if (isConected)
                {
                    // Solo este usuario puede escribir
                    if (!strcmp(header, "UABC"))
                    {
                        if (!strcmp(user, "OAC"))
                        {
                            // Solo lectura
                            if (operation == 'R')
                            {
                                if (element == 'L')
                                {
                                    // Paquete TCP
                                    sendLedState(sockServer, ledState);
                                }
                                else if (element == 'A')
                                {
                                    // Paquete TCP
                                    sendADCState(sockServer);
                                }
                                else
                                {
                                    // Error
                                    send(sockServer, nack, strlen(nack), 0);
                                    ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                                }
                            }
                            // Solo escritura
                            else if (operation == 'W')
                            {
                                if (element == 'L')
                                {
                                    if (value == '1' || value == '0')
                                    {
                                        // Paquete TCP
                                        ledState[0] = setLedState(sockServer, value);
                                    }
                                    else
                                    {
                                        // Error
                                        send(sockServer, nack, strlen(nack), 0);
                                        ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                                    }
                                }
                                else
                                {
                                    // Error
                                    send(sockServer, nack, strlen(nack), 0);
                                    ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                                }
                            }
                            else
                            {
                                // Error
                                send(sockServer, nack, strlen(nack), 0);
                                ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                            }
                        }
                        else
                        {
                            // Error
                            send(sockServer, nack, strlen(nack), 0);
                            ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                        }
                    }
                    else
                    {
                        // Error
                        send(sockServer, nack, strlen(nack), 0);
                        ESP_LOGI("SERVER", "Respuesta al server:%s", nack);
                    }
                }
            }
            else
            {
                ESP_LOGI(TAG, "No se recibieron datos");
            }
            isWaiting = 0;
            vTaskDelay(100);
        }
        // Solo se llega aquí en caso de error
        ESP_LOGE(TAG, "Cerrando los sockets y reiniciando...");
        shutdown(sockServer, 0);
        close(sockServer);*/
    }
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