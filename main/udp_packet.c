#include "udp_packet.h"

int addr_family = AF_INET;
int ip_protocol = 0;

int create_UDP_socket(int port)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);

    if (sock < 0)
    {
        ESP_LOGE("UDP", "Error al crear los sockets: errno %d", errno);
    }
    ESP_LOGI("UDP", "Sockets creados!");

    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

    int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0)
    {
        ESP_LOGE("UDP", "El socket no se puede enlazar: errno %d", errno);
        return -1;
    }
    ESP_LOGI("UDP", "Socket enlazado en el puerto, port %d", port);
    return sock;
}

char setLedStateUDP(int sock, char state, struct sockaddr_storage source_addr)
{
    char tmp[2];
    tmp[0] = state;
    tmp[1] = '\0';
    strcat(ack, tmp);
    gpio_set_level(LED, (state == '1') ? HIGH : LOW);
    sendto(sock, ack, sizeof(ack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
    return state;
}

void sendLedStateUDP(int sock, char ledState[2], struct sockaddr_storage source_addr)
{
    strcat(ack, ledState);
    sendto(sock, ack, sizeof(ack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
}

int sendADCStateUDP(int sock, struct sockaddr_storage source_addr)
{
    int adcVal = adc1_get_raw(ADC1_CHANNEL_5);
    char val[6];
    sprintf(val, "%d", adcVal);
    strcat(ack, val);
    sendto(sock, ack, sizeof(ack), 0, (struct sockaddr *)&source_addr, sizeof(struct sockaddr_in));
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
    return adcVal;
}
