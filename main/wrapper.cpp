#include "wrapper.h"
#include "MySQL_Connection.h"

MySQL_Connection* mysql_connection_new() {
    return new MySQL_Connection();  // Crear la instancia directamente
}

void mysql_connection_delete(MySQL_Connection* conn) {
    delete conn;  // Eliminar la instancia directamente
}

int mysql_connection_connect(MySQL_Connection* conn, const char* server, int port, 
                             const char* user, const char* password, const char* db) {
    return conn->connect(server, port, user, password, db);  // Llamar directamente a `connect`
}

void mysql_connection_close(MySQL_Connection* conn) {
    conn->close();  // Llamar directamente a `close`
}