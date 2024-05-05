#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "data_storage.h"

#define PORT 5678
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Erstellt einen Socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setzt Optionen für den Socket
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Konfiguriert Adressenstruktur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bindet den Socket an die Adresse
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Hört auf eingehende Verbindungen
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // Wartet auf Verbindungen und Verarbeiten von Anfragen
    while (1) {
        printf("Waiting for connection...\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Lesen von Daten vom Client
        valread = read(new_socket, buffer, BUFFER_SIZE);
        buffer[valread] = '\0'; // Nullterminierung

        // Tokens des Befehls
        char *command = strtok(buffer, " ");
        char *key = strtok(NULL, " ");
        char *value = strtok(NULL, " ");

        // Ausführen des Befehls und Senden der Antwort
        if (strcmp(command, "GET") == 0) {
            char result[BUFFER_SIZE];
            int ret = get(key, result);
            if (ret < 0)
                sprintf(result, "GET:%s:key_nonexistent", key);
            send(new_socket, result, strlen(result), 0);
        } else if (strcmp(command, "PUT") == 0) {
            char result[BUFFER_SIZE];
            int ret = put(key, value);
            sprintf(result, "PUT:%s:%s", key, value);
            send(new_socket, result, strlen(result), 0);
        } else if (strcmp(command, "DEL") == 0) {
            char result[BUFFER_SIZE];
            int ret = del(key);
            if (ret == 0)
                sprintf(result, "DEL:%s:key_deleted", key);
            else
                sprintf(result, "DEL:%s:key_nonexistent", key);
            send(new_socket, result, strlen(result), 0);
        } else if (strcmp(command, "QUIT") == 0) {
            close(new_socket);
            break;
        }

        // Zurücksetzen des Puffers
        memset(buffer, 0, BUFFER_SIZE);
    }

    return 0;
}
