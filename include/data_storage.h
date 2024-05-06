#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#define MAX_KEY_LENGTH 100
#define MAX_VALUE_LENGTH 100

typedef struct {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
} KeyValuePair;

int put(char* key, char* value);
int get(char* key, char* res);
int del(char* key);

#endif /* DATA_STORAGE_H */
