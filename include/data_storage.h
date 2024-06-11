#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <pthread.h>
#include <definitions.h>

#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100
#define MAX_ENTRIES 1000
#define MAX_SUBSCRIBERS 100

typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} KeyValuePair;


typedef struct {
    char key[BUFFER_SIZE];
    int clients[MAX_SUBSCRIBERS];
    int num_clients;
} Subscription;

// External variables
extern KeyValuePair data[MAX_ENTRIES];
extern int num_entries;
extern pthread_mutex_t mutex;

// Function prototypes
int put(char *key, char *value);
int get(char *key, char *res);
int del(char *key);
int sub(const char *key, int client);
void pub(const char *key, char u);


#endif /* DATA_STORAGE_H */
