#ifndef MYSQL_CONNECTION_H
#define MYSQL_CONNECTION_H

#include "lwip/sockets.h"

class MySQL_Connection {
public:
    MySQL_Connection() : sockfd(-1) {}  // Constructor

    bool connect(const char* server, int port, const char* user,
                 const char* password, const char* db = nullptr);
    
    int connected() { return sockfd != -1; }  // Verificar el estado de la conexión
    //const char* version() { return MYSQL_VERSION_STR; }
    void close();

private:
    int sockfd;  // Descriptor del socket
};

#endif
