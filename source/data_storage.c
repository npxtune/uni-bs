#include "../include/data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES 1000

KeyValuePair data[MAX_ENTRIES];
int num_entries = 0;

int put(char *key, char *value) {
    if (value == '\0') { return EXIT_FAILURE; }

    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            strcpy(data[i].value, value);
            return EXIT_SUCCESS; // Success, key found and value updated
        }
    }

    if (num_entries < MAX_ENTRIES) {
        strcpy(data[num_entries].key, key);
        strcpy(data[num_entries].value, value);
        num_entries++;
        return EXIT_SUCCESS; // Success, new key-value pair added
    }

    return EXIT_FAILURE; // Failure, maximum entries reached
}

int get(char *key, char *res) {
    for (int i = 0; i < sizeof(key); i++) {
        if (isdigit(key[i])) {
            key[i + 1] = '\0';
            break;
        }
    }
    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            strcpy(res, data[i].value);
            return 1; // Success, key found
        }
    }
    return -1; // Failure, key not found
}

int del(char *key) {
    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            // Shift elements to cover deleted entry
            for (int j = i; j < num_entries - 1; ++j) {
                strcpy(data[j].key, data[j + 1].key);
                strcpy(data[j].value, data[j + 1].value);
            }
            num_entries--;
            return 0; // Success, key deleted
        }
    }
    return -1; // Failure, key not found
}
