#include "query.h"
#include "data_storage.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// Enum for command strings
enum command_strings { get_command, put_command, del_command, dc_command, quit_command, beg_command, end_command, sub_command };

const lookup_t COMMANDS[] = {
        {"GET"},
        {"PUT"},
        {"DEL"},
        {"DC"},
        {"QUIT"},
        {"BEG"},
        {"END"},
        {"SUB"}
};

int begClient = -1; // Initialize begClient to -1 to indicate no exclusive client

int query(const client_data *data, const int *client) {
    for (int i = 0; i < NELEMS(COMMANDS); i++) {
        if (strncmp(COMMANDS[i].cmd, data->command, strlen(data->command)) == 0) {
            return exec(i, data, client);
        }
    }
    return EXIT_FAILURE;
}

int exec(const int commandIndex, const client_data *data, const int *client) {
    switch (commandIndex) {
        case get_command: {
            if(*client == begClient || begClient == -1) {
                char result[BUFFER_SIZE];
                const int ret = get(data->key, result);
                int temp = 0;
                for (int i = 0; i < sizeof(data->key); i++) {
                    if (isdigit(data->key[i])) temp = data->key[i] - '0';
                }
                if (ret < 0) {
                    snprintf(result, sizeof(result) - 2, "\033[31mkey%d:\tKey not found!\033[0m\n", temp);
                } else {
                    send(*client, result, strlen(result), 0);
                    pub(data->key, 'g');
                }
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "beg started by other client");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        case put_command: {
            if(*client == begClient || begClient == -1) {
                char result[BUFFER_SIZE];
                if (put(data->key, data->value) == EXIT_SUCCESS) {
                    snprintf(result, sizeof(result), "%s -> %s", data->key, data->value);
                    send(*client, result, strlen(result), 0);
                    pub(data->key, 'p');
                } else {
                    snprintf(result, sizeof(result), "\033[31mNo value provided!\033[0m\n");
                    send(*client, result, strlen(result), 0);
                }
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "beg started by other client");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        case del_command: {
            if(*client == begClient || begClient == -1) {
                char result[BUFFER_SIZE];
                const int ret = del(data->key);
                if (ret == 0) {
                    snprintf(result, sizeof(result), "%s's value deleted.\n", data->key);
                    send(*client, result, strlen(result), 0);
                    pub(data->key, 'd'); //key wird übermittelt und status d für delete wird übermittelt
                } else {
                    snprintf(result, sizeof(result), "\033[31m%s has no value for deletion.\033[0m\n", data->key);
                    send(*client, result, strlen(result), 0);
                }
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "beg started by other client");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        case dc_command: {
            printf("Client %d requested disconnect\n", *client);
            return CLIENT_DISCONNECT;
        }

        case quit_command: {
            printf("Client %d requested shutdown\n", *client);
            return SERVER_SHUTDOWN;
        }

        case beg_command: {
            if (begClient == -1) {
                begClient = *client;
                printf("Client %d is now key_value_store_main_access\n", begClient);
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "Another client already has exclusive access");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        case end_command: {
            if (begClient == *client) {
                printf("Client %d has left key_value_store_main_access\n", *client);
                begClient = -1;
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "You do not have exclusive access");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        case sub_command: {
            if (*client == begClient || begClient == -1) {
                if (sub(data->key, *client) == 0) {
                    char result[BUFFER_SIZE];
                    snprintf(result, sizeof(result), "Subscribed to %s\n", data->key);
                    send(*client, result, strlen(result), 0);
                } else {
                    char result[BUFFER_SIZE];
                    snprintf(result, sizeof(result), "Failed to subscribe to %s\n", data->key);
                    send(*client, result, strlen(result), 0);
                }
            } else {
                char result[BUFFER_SIZE];
                snprintf(result, sizeof(result), "beg started by other client");
                send(*client, result, strlen(result), 0);
            }
            return EXIT_SUCCESS;
        }

        default: {
            char result[BUFFER_SIZE];
            snprintf(result, sizeof(result), "\033[31mInvalid command.\033[0m\n");
            send(*client, result, strlen(result), 0);
            return EXEC_FAILURE;
        }
    }
}
