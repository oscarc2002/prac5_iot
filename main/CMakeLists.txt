idf_component_register(SRCS "main.c" "tcp_packet.c" "wifi_connection.c" "http_request.c" "MySQL_Connection.cpp" "wrapper.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES driver esp_http_client esp_event esp_wifi nvs_flash)