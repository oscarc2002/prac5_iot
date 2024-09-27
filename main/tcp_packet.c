#include "tcp_packet.h"

char ack[16] = "ACK:";

esp_err_t configGPIOS()
{
    gpio_reset_pin(LED);
    gpio_reset_pin(VCC);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(VCC, GPIO_MODE_OUTPUT);
    gpio_set_level(VCC, HIGH);
    adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_0);
    adc1_config_width(ADC_WIDTH_BIT_12);
    return ESP_OK;
}

char setLedState(int sock, char state)
{
    char tmp[2];
    tmp[0] = state;
    tmp[1] = '\0';
    strcat(ack, tmp);
    gpio_set_level(LED, (state == '1') ? HIGH : LOW);
    send(sock, ack, strlen(ack), 0);
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
    return state;
}

void resetACK()
{
    memset(ack, 0, sizeof(ack));
    strcpy(ack, "ACK:");
}

void sendLedState(int sock, char ledState[2])
{
    strcat(ack, ledState);
    send(sock, ack, strlen(ack), 0);
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
}

void sendADCState(int sock)
{
    int adcVal = adc1_get_raw(ADC1_CHANNEL_5);
    char val[6];
    sprintf(val, "%d", adcVal);
    strcat(ack, val);
    send(sock, ack, strlen(ack), 0);
    ESP_LOGW("SERVER", "Respuesta al server:%s", ack);
    resetACK();
}
