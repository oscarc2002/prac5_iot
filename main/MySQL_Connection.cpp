#include "MySQL_Connection.h"
#include "esp_log.h"
#include "lwip/sockets.h"
#include "string.h"

#define MAX_CONNECT_ATTEMPTS 3
#define CONNECT_DELAY_MS     500
#define SUCCESS              1

static const char* TAG = "MySQL_Connection";

bool MySQL_Connection::connect(const char* server, int port, const char* user,
                 const char* password, const char* db) {
    int connected = 0;
    int retries = MAX_CONNECT_ATTEMPTS;

    while (retries--) {
        ESP_LOGI(TAG, "...trying...");
        
        struct sockaddr_in server_addr;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            ESP_LOGE(TAG, "Socket creation failed");
            return false;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, server, &server_addr.sin_addr);

        connected = ::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (connected < 0) {
            ESP_LOGE(TAG, "...connection failed, got: %d, retrying...", connected);
            close(); // Cerrar el socket si falla
            vTaskDelay(CONNECT_DELAY_MS / portTICK_PERIOD_MS);
        } else {
            break;
        }
    }

    if (connected < 0) {
        return false;
    }

    // Aquí deberías llamar a tus métodos para manejar la autenticación y el paquete handshake
    ESP_LOGI(TAG, "Connected to server version %s", "server_version_placeholder"); // Cambiar según sea necesario
    return true;
}

void MySQL_Connection::close() {
    if (connected()) {
        shutdown(sockfd, 0);
        ::close(sockfd);
        sockfd = -1;
        ESP_LOGI(TAG, "Disconnected.");
    }
}
