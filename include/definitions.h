
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define BUFFER_SIZE 1024
#define CLIENT_DISCONNECT -1
#define SERVER_SHUTDOWN -2
#define EXEC_FAILURE -3
#define PORT 5678
#define MAX_CLIENTS 30

typedef struct {
    char* cmd;
} lookup_t;

typedef struct {
    const char *command;
    char *key;
    char *value;
} client_data;

#endif //DEFINITIONS_H
