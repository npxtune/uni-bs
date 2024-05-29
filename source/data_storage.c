// data_storage.c
#include "../include/data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <definitions.h>

#define MAX_ENTRIES 1000
#define MAX_SUBSCRIBERS 100

typedef struct {
    char key[256];
    int clients[MAX_SUBSCRIBERS];
    int num_subscribers;
} Subscription;

KeyValuePair data[MAX_ENTRIES];
int num_entries = 0;
Subscription subscriptions[MAX_ENTRIES];
int num_subscriptions = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

void notify_subscribers(const char *key, const char *value) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < num_subscriptions; ++i) {
        if (strcmp(subscriptions[i].key, key) == 0) {
            for (int j = 0; j < subscriptions[i].num_subscribers; ++j) {
                int client = subscriptions[i].clients[j];
                char message[BUFFER_SIZE];
                snprintf(message, sizeof(message), "Update: %s -> %s\n", key, value);
                send(client, message, strlen(message), 0);
            }
            break;
        }
    }
    pthread_mutex_unlock(&mutex);
}

int put(char *key, char *value) {
    pthread_mutex_lock(&mutex); // Lock mutex before accessing shared resources

    if (value == NULL) {
        pthread_mutex_unlock(&mutex); // Unlock mutex before returning
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_entries; ++i) {
        if (strcmp(data[i].key, key) == 0) {
            strcpy(data[i].value, value);
            notify_subscribers(key, value);
            pthread_mutex_unlock(&mutex); // Unlock mutex before returning
            return EXIT_SUCCESS; // Success, key found and value updated
        }
    }

    if (num_entries < MAX_ENTRIES) {
        strcpy(data[num_entries].key, key);
        strcpy(data[num_entries].value, value);
        num_entries++;
        notify_subscribers(key, value);
        pthread_mutex_unlock(&mutex); // Unlock mutex before returning
        return EXIT_SUCCESS; // Success, new key-value pair added
    }

    pthread_mutex_unlock(&mutex); // Unlock mutex before returning
    return EXIT_FAILURE; // Failure, maximum entries reached
}

int subscribe(int client, const char *key) {
    pthread_mutex_lock(&mutex);

    for (int i = 0; i < num_subscriptions; ++i) {
        if (strcmp(subscriptions[i].key, key) == 0) {
            if (subscriptions[i].num_subscribers < MAX_SUBSCRIBERS) {
                subscriptions[i].clients[subscriptions[i].num_subscribers++] = client;
                pthread_mutex_unlock(&mutex);
                return EXIT_SUCCESS;
            }
            pthread_mutex_unlock(&mutex);
            return EXIT_FAILURE; // Too many subscribers
        }
    }

    if (num_subscriptions < MAX_ENTRIES) {
        strcpy(subscriptions[num_subscriptions].key, key);
        subscriptions[num_subscriptions].clients[0] = client;
        subscriptions[num_subscriptions].num_subscribers = 1;
        num_subscriptions++;
        pthread_mutex_unlock(&mutex);
        return EXIT_SUCCESS;
    }

    pthread_mutex_unlock(&mutex);
    return EXIT_FAILURE; // Maximum subscriptions reached
}
