#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "data_storage.h"

#define PORT 5678
#define BUFFER_SIZE 1024

bool IsClientConnected = false;

int main() {
    int serverFd, newSocket = 0;
    struct sockaddr_in address;
    const int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Erstellt einen Socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setzt Optionen für den Socket
    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Konfiguriert Adressenstruktur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bindet den Socket an die Adresse
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Hört auf eingehende Verbindungen
    if (listen(serverFd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    while(!IsClientConnected) {
        printf("Waiting for connection...\n");
        if ((newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        IsClientConnected = true;
    }

    // Wartet auf Verbindungen und Verarbeiten von Anfragen
    while (1) {
        // Lesen von Daten vom Client
        const int valread = read(newSocket, buffer, BUFFER_SIZE);
        buffer[valread] = '\0'; // Nullterminierung

        printf("Client sent command: %s", buffer);

        // Tokens des Befehls
        const char *command = strtok(buffer, " ");
        char *key = strtok(NULL, " ");
        char *value = strtok(NULL, " ");

        // Ausführen des Befehls und Senden der Antwort
        if (strcmp(command, "QUIT") == 13) {    // Wieso 13? Nur Gott weiß wieso...
            goto EXIT_CLEANUP;
        }
        if (strcmp(command, "GET") == 0) {
            char result[BUFFER_SIZE];
            const int ret = get(key, result);
            if (ret < 0)
                snprintf(result, sizeof(result)-2, "GET:%s:key_nonexistent", key);
            send(newSocket, result, strlen(result), 0);
        }
        if (strcmp(command, "PUT") == 0) {
            char result[BUFFER_SIZE];
            put(key, value);
            sprintf(result, "PUT:%s:%s", key, value);
            send(newSocket, result, strlen(result), 0);
        }
        if (strcmp(command, "DEL") == 0) {
            char result[BUFFER_SIZE];
            const int ret = del(key);
            if (ret == 0)
                sprintf(result, "DEL:%s:key_deleted", key);
            else
                sprintf(result, "DEL:%s:key_nonexistent", key);
            send(newSocket, result, strlen(result), 0);
        }

        // Zurücksetzen des Puffers
        memset(buffer, 0, BUFFER_SIZE);
    }
    EXIT_CLEANUP:
    memset(buffer, 0, BUFFER_SIZE);
    close(newSocket);
    close(serverFd);
    IsClientConnected = false;
    exit(EXIT_SUCCESS);
    return 0;
}