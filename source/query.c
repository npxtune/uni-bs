// query.c
#include "query.h"
#include "data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

enum command_strings {get_command, put_command, del_command, dc_command, quit_command, sub_command};

const lookup_t COMMANDS[] = {
        {"GET"},
        {"PUT"},
        {"DEL"},
        {"DC"},
        {"QUIT"},
        {"SUB"},
};

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
        // ... existing cases ...

        case sub_command: {
            char result[BUFFER_SIZE];
            if (subscribe(*client, data->key) == EXIT_SUCCESS) {
                snprintf(result, sizeof(result), "Subscribed to key %s\n", data->key);
            } else {
                snprintf(result, sizeof(result), "\033[31mFailed to subscribe to key %s\033[0m\n", data->key);
            }
            send(*client, result, strlen(result), 0);
            return EXIT_SUCCESS;
        }

            // ... existing cases ...
    }
}
