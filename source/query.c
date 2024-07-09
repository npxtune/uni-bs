#include "query.h"
#include "data_storage.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

typedef enum {
    GET_COMMAND,
    PUT_COMMAND,
    DEL_COMMAND,
    DC_COMMAND,
    QUIT_COMMAND,
    BEG_COMMAND,
    END_COMMAND,
    SUB_COMMAND,
    INVALID_COMMAND
} command_t;

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
        case GET_COMMAND:
            return handle_get(data, client);
        case PUT_COMMAND:
            return handle_put(data, client);
        case DEL_COMMAND:
            return handle_del(data, client);
        case DC_COMMAND:
            return handle_dc(client);
        case QUIT_COMMAND:
            return handle_quit(client);
        case BEG_COMMAND:
            return handle_beg(client);
        case END_COMMAND:
            return handle_end(client);
        case SUB_COMMAND:
            return handle_sub(data, client);
        default:
            char result[BUFFER_SIZE];
            snprintf(result, sizeof(result), "\033[31mInvalid command.\033[0m\n");
            send(*client, result, strlen(result), 0);
            return EXIT_FAILURE;
    }
}

int handle_get(const client_data *data, const int *client) {
    char result[BUFFER_SIZE];
    if(*client == begClient || begClient == -1) {
        const int ret = get(data->key, result);
        int temp = 0;
        for (int i = 0; i < sizeof(data->key); i++) {
            if (isdigit(data->key[i])) temp = data->key[i] - '0';
        }
        if (ret < 0) {
            snprintf(result, sizeof(result) - 2, "\033[31mkey%d:\tKey not found!\033[0m\n", temp);
        } else {
            send(*client, result, strlen(result), 0);
            pub(data->key, GET_COMMAND, *client);
        }
    } else {
        snprintf(result, sizeof(result), "beg started by other client");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}

int handle_put(const client_data *data, const int *client) {
    char result[BUFFER_SIZE];
    if(*client == begClient || begClient == -1) {
        if (put(data->key, data->value) == EXIT_SUCCESS) {
            snprintf(result, sizeof(result), "%s -> %s", data->key, data->value);
            send(*client, result, strlen(result), 0);
            pub(data->key, PUT_COMMAND, *client);
        } else {
            snprintf(result, sizeof(result), "\033[31mNo value provided!\033[0m\n");
            send(*client, result, strlen(result), 0);
        }
    } else {
        snprintf(result, sizeof(result), "beg started by other client");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}

int handle_del(const client_data *data, const int *client) {
    char result[BUFFER_SIZE];
    if(*client == begClient || begClient == -1) {
        const int ret = del(data->key);
        if (ret == 0) {
            snprintf(result, sizeof(result), "%s's value deleted.\n", data->key);
            send(*client, result, strlen(result), 0);
            pub(data->key, DEL_COMMAND, *client);
        } else {
            snprintf(result, sizeof(result), "\033[31m%s has no value for deletion.\033[0m\n", data->key);
            send(*client, result, strlen(result), 0);
        }
    } else {
        snprintf(result, sizeof(result), "beg started by other client");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}

int handle_dc(const int *client) {
    printf("Client %d requested disconnect\n", *client);
    return CLIENT_DISCONNECT;
}

int handle_quit(const int *client) {
    printf("Client %d requested shutdown\n", *client);
    return SERVER_SHUTDOWN;
}

int handle_beg(const int *client) {
    char result[BUFFER_SIZE];
    if (begClient == -1) {
        begClient = *client;
        printf("Client %d is now key_value_store_main_access\n", begClient);
    } else {
        snprintf(result, sizeof(result), "Another client already has exclusive access");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}

int handle_end(const int *client) {
    char result[BUFFER_SIZE];
    if (begClient == *client) {
        printf("Client %d has left key_value_store_main_access\n", *client);
        begClient = -1;
    } else {
        snprintf(result, sizeof(result), "You do not have exclusive access");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}

int handle_sub(const client_data *data, const int *client) {
    char result[BUFFER_SIZE];
    if (*client == begClient || begClient == -1) {
        if (sub(data->key, *client) == 0) {
            snprintf(result, sizeof(result), "Subscribed to %s\n", data->key);
            send(*client, result, strlen(result), 0);
        } else {
            snprintf(result, sizeof(result), "Failed to subscribe to %s\n", data->key);
            send(*client, result, strlen(result), 0);
        }
    } else {
        snprintf(result, sizeof(result), "beg started by other client");
        send(*client, result, strlen(result), 0);
    }
    return EXIT_SUCCESS;
}
