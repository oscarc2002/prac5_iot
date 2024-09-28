#include "custom_mysql.h"

int connect_to_mysql(const char* ip, int port) {
    struct sockaddr_in server_addr;
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        ESP_LOGE("MySQL", "Error creando socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE("MySQL", "Error conectando al servidor");
        close(sock);
        return -1;
    }
    else{
        ESP_LOGI("MySQL", "Se conectó al servidor de MySQL");
    }

    return sock;
}

void send_query(int sock, const char* query) {
    char rx_buffer[128];
    send(sock, query, strlen(query), 0);
    int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (len > 0) {
        ESP_LOGI("MySQL","Mensaje del server:%s",rx_buffer);
    }
    else{
        ESP_LOGE("MySQL","No regresó nada el server");
    }
    
}

void send_data(int sock, const char* device, const char* ip, const char* rssi, const char* led, const char* adc) {
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO data (Device, IP, RSSI, LED, ADC) VALUES ('%s', '%s', %s, '%s', '%s');",
             device, ip, rssi, led, adc);
    printf("Enviar informacion\n");
    send_query(sock, query);
}