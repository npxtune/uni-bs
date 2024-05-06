#include "init.h"
#include "data_storage.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <error.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 1024

bool IsClientConnected = false;
int Server, Client = 0;
struct sockaddr_in Address;
char Buffer[BUFFER_SIZE] = {0};

int main() {
    if (init(&Server, &Address) == EXIT_SUCCESS) {
        printf("Opened Socket on Port: %d\n", 5678);
    } else {
        perror("FAILED TO OPEN SOCKET!\n");
        return -1;
    }
    socklen_t length = sizeof(Address);

WAITING_CONNECT:
    while (!IsClientConnected) {
        printf("Waiting for connection...\n");
        if ((Client = accept(Server, (struct sockaddr*)&Address, (socklen_t*)&length)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        IsClientConnected = true;
    }

    // Wartet auf Verbindungen und Verarbeiten von Anfragen
LOOP:
    1;
    // Lesen von Daten vom Client
    const int valread = read(Client, Buffer, BUFFER_SIZE);

    for (int i = 0; i < sizeof(Buffer); i++) {
        // Make non case sensitive
        Buffer[i] = toupper(Buffer[i]);
    }
    Buffer[valread] = '\0'; // Nullterminierung

    printf("Client sent command: %s", Buffer);

    // Tokens des Befehls
    const char *command = strtok(Buffer, " ");
    char *key = strtok(NULL, " ");
    char *value = strtok(NULL, " ");

    // Ausführen des Befehls und Senden der Antwort
    if (strcmp(command, "GET") == 0) {
        char result[BUFFER_SIZE];
        const int ret = get(key, result);
        int temp = 0;
        for (int i = 0; i < sizeof(key); i++) {
            if (isdigit(key[i])) temp = key[i] - '0';
        }
        if (ret < 0) {
            snprintf(result, sizeof(result) - 2, "\033[31mkey%d:\tKey not found!\033[0m\n", temp);
        }
        send(Client, result, strlen(result), 0);
    }

    if (strcmp(command, "PUT") == 0) {
        char result[BUFFER_SIZE];
        if (put(key, value) == EXIT_SUCCESS) {
            sprintf(result, "%s -> %s", key, value);
            send(Client, result, strlen(result), 0);
        } else {
            snprintf(result, sizeof(result), "\033[31mNO VALUE FOR KEY PROVIDED!\033[0m\n");
            send(Client, result, strlen(result), 0);
        }
    }

    if (strcmp(command, "DEL") == 0) {
        char result[BUFFER_SIZE];
        const int ret = del(key);
        if (ret == 0)
            sprintf(result, "DEL:%s:key_deleted", key);
        else
            sprintf(result, "DEL:%s:key_nonexistent", key);
        send(Client, result, strlen(result), 0);
    }

    if (strcmp(command, "DC") == 0) {
        memset(Buffer, 0, BUFFER_SIZE);
        printf("Closing Socket %d\n", Client);
        close(Client);
        IsClientConnected = false;
        goto WAITING_CONNECT;
    }

    if (strcmp(command, "QUIT") == 13 || strcmp(command, "Q") == 13) {
        // Wieso 13? Nur Gott weiß wieso...
        goto EXIT_CLEANUP;
    }

    // Zurücksetzen des Puffers
    memset(Buffer, 0, BUFFER_SIZE);
    goto LOOP;

    //  ----------------------------------------------------------------

EXIT_CLEANUP:
    memset(Buffer, 0, BUFFER_SIZE);
    close(Client);
    close(Server);
    IsClientConnected = false;
    exit(EXIT_SUCCESS);
    return 0;
}
