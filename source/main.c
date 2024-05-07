#include "init.h"
#include "data_storage.h"
#include "query.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

bool IsClientConnected = false;
int Server, Client = 0;
struct sockaddr_in Address;
char Buffer[BUFFER_SIZE] = {0};

int main() {
    if (init(&Server, &Address) == EXIT_SUCCESS) {
        // Initialize Socket
        printf("Opened Socket on Port: %d\n", 5678);
    }
    else {
        perror("FAILED TO OPEN SOCKET!\n");
        return -1;
    }

WAITING_CONNECT:
    while (!IsClientConnected) {
        // Wait for Client connection
        socklen_t length = sizeof(Address);
        printf("Waiting for connection...\n");
        if ((Client = accept(Server, (struct sockaddr*)&Address, &length)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        IsClientConnected = true;
    }


    // Read input from client
    while (IsClientConnected) {
        const int valread = read(Client, Buffer, BUFFER_SIZE);

        for (int i = 0; i < sizeof(Buffer); i++) {
            // Make non case sensitive
            Buffer[i] = toupper(Buffer[i]);
        }
        Buffer[valread] = '\0'; // NULL char

        printf("Client sent command: %s", Buffer); // For debugging :)

        const client_data data = {strtok(Buffer, " "), strtok(NULL, " "), strtok(NULL, " ")};

        const int result = query(&data, &Client);

        if (result == SERVER_SHUTDOWN) {
            IsClientConnected = false;
        } else if (result == CLIENT_DISCONNECT) {
            memset(Buffer, 0, BUFFER_SIZE);
            close(Client);
            IsClientConnected = false;
            goto WAITING_CONNECT;
        }

        // Reset Buffer
        memset(Buffer, 0, BUFFER_SIZE);
    }
    close(Client);
    close(Server);
    exit(EXIT_SUCCESS);
}
