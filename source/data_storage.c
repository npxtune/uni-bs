#include "../include/data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_ENTRIES 1000

KeyValuePair data[MAX_ENTRIES];
int num_entries = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

int put(char *key, char *value) {
    pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources

    if (value == NULL) {
        pthread_mutex_unlock(&mutex); // Unlock mutex before returning
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            strcpy(data[i].value, value);
            pthread_mutex_unlock(&mutex); // Unlock mutex before returning
            return EXIT_SUCCESS; // Success, key found and value updated
        }
    }

    if (num_entries < MAX_ENTRIES) {
        strcpy(data[num_entries].key, key);
        strcpy(data[num_entries].value, value);
        num_entries++;
        pthread_mutex_unlock(&mutex); // Unlock mutex before returning
        return EXIT_SUCCESS; // Success, new key-value pair added
    }

    pthread_mutex_unlock(&mutex); // Unlock mutex before returning
    return EXIT_FAILURE; // Failure, maximum entries reached
}

int get(char *key, char *res) {
    pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources

    for (int i = 0; i < sizeof(key); i++) {
        if (isdigit(key[i])) {
            key[i + 1] = '\0';
            break;
        }
    }
    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            strcpy(res, data[i].value);
            pthread_mutex_unlock(&mutex); // Unlock mutex before returning
            return 1; // Success, key found
        }
    }
    pthread_mutex_unlock(&mutex); // Unlock mutex before returning
    return -1; // Failure, key not found
}

int del(char *key) {
    pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources

    for (int i = 0; i < sizeof(key); i++) {
        if (isdigit(key[i])) {
            key[i + 1] = '\0';
            break;
        }
    }
    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            // Shift elements to cover deleted entry
            for (int j = i; j < num_entries - 1; ++j) {
                strcpy(data[j].key, data[j + 1].key);
                strcpy(data[j].value, data[j + 1].value);
            }
            num_entries--;
            pthread_mutex_unlock(&mutex); // Unlock mutex before returning
            return 0; // Success, key deleted
        }
    }
    pthread_mutex_unlock(&mutex); // Unlock mutex before returning
    return -1; // Failure, key not found
}
