#ifndef WRAPPER_H
#define WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MySQL_Connection MySQL_Connection;

MySQL_Connection* mysql_connection_new();
void mysql_connection_delete(MySQL_Connection* conn);
int mysql_connection_connect(MySQL_Connection* conn, const char* server, int port, const char* user,
                             const char* password, const char* db);
void mysql_connection_close(MySQL_Connection* conn);

#ifdef __cplusplus
}
#endif

#endif // WRAPPER_H
