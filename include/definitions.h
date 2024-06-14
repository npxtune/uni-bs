
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define BUFFER_SIZE 1024
#define CLIENT_DISCONNECT -1
#define SERVER_SHUTDOWN -2
#define EXEC_FAILURE -3
#define PORT 5678
#define MAX_CLIENTS 30

// Define a function to handle client requests
void *handle_client(void *arg);

typedef struct {
    char* cmd;
} lookup_t;

typedef struct {
    const char *command;
    char *key;
    char *value;
} client_data;

// Enum for command strings
enum command_strings { get_command, put_command, del_command, dc_command, quit_command, beg_command, end_command, sub_command };

#endif //DEFINITIONS_H
