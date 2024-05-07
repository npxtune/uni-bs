#include "query.h"
#include "data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

const lookup_t COMMANDS[] = {
    {"GET"},
    {"PUT"},
    {"DEL"},
    {"DC"},
    {"QUIT"},
};

int query(const client_data *data, const int *client) { // Query the client's data against internal command list
    for (int i = 0; i < NELEMS(COMMANDS); i++) {
        if (strncmp(COMMANDS[i].word, data->command, strlen(data->command)) == 0) {
            return exec(i, data, client);   // If found, execute corresponding command
        }
    }
    return EXIT_FAILURE; // Command not found
}

int exec(const int commandIndex, const client_data *data, const int *client) {
    switch (commandIndex) {
    // Execute command

    case get_command: {
        char result[BUFFER_SIZE];
        const int ret = get(data->key, result);
        int temp = 0;
        for (int i = 0; i < sizeof(data->key); i++) {
            if (isdigit(data->key[i])) temp = data->key[i] - '0';
        }
        if (ret < 0) {
            snprintf(result, sizeof(result) - 2, "\033[31mkey%d:\tKey not found!\033[0m\n", temp);
        }
        send(*client, result, strlen(result), 0);
        return EXIT_SUCCESS;
    }

    case put_command: {
        char result[BUFFER_SIZE];
        if (put(data->key, data->value) == EXIT_SUCCESS) {
            sprintf(result, "%s -> %s", data->key, data->value);
            send(*client, result, strlen(result), 0);
        }
        else {
            snprintf(result, sizeof(result), "\033[31mNo value provided!\033[0m\n");
            send(*client, result, strlen(result), 0);
        }
        return EXIT_SUCCESS;
    }

    case del_command: {
        char result[BUFFER_SIZE];
        const int ret = del(data->key);
        if (ret == 0)
            sprintf(result, "%s's value deleted.\n", data->key);
        else
            sprintf(result, "\033[31m%s has no value for deletion.\033[0m\n", data->key);
        send(*client, result, strlen(result), 0);
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

    default: {  // Should never be reached but meh
        char result[BUFFER_SIZE];
        sprintf(result, "\033[31mInvalid command.\033[0m\n");
        send(*client, result, strlen(result), 0);
        return EXEC_FAILURE;
    }
    }
}
