#include "init.h"
#include "data_storage.h"
#include "query.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

bool IsClientConnected = false;
int Server;
struct sockaddr_in Address;
char Buffer[BUFFER_SIZE] = {0};

// Define a function to handle client requests
void *handle_client(void *arg);

int main() {
    if (init(&Server, &Address) == EXIT_SUCCESS) {
        // Initialize Socket
        printf("Opened Socket on Port: %d\n", PORT);
    }
    else {
        perror("FAILED TO OPEN SOCKET!\n");
        return -1;
    }

    while (true) {
        socklen_t length = sizeof(Address);
        int *client = malloc(sizeof(int)); // Allocate memory for client socket descriptor
        if ((*client = accept(Server, (struct sockaddr*)&Address, &length)) < 0) {
            perror("Accept failed");
            free(client); // Free allocated memory before exiting
            continue;
        }

        // Create a thread to handle client requests
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, (void *)client);
        pthread_detach(tid); // Detach the thread to avoid memory leaks
    }

    close(Server);
    return 0;
}

void *handle_client(void *arg) {
    int client = *((int *)arg); // Cast void pointer back to int pointer and dereference
    free(arg); // Free memory allocated for argument

    // Read input from client
    while (true) {
        const int valread = read(client, Buffer, BUFFER_SIZE);

        for (int i = 0; i < sizeof(Buffer); i++) {
            // Make non case sensitive
            Buffer[i] = toupper(Buffer[i]);
        }
        Buffer[valread] = '\0'; // NULL char

        printf("Client %d sent command: %s", client, Buffer); // For debugging :)

        const client_data data = {strtok(Buffer, " "), strtok(NULL, " "), strtok(NULL, " ")};

        const int result = query(&data, &client);

        if (result == SERVER_SHUTDOWN || result == CLIENT_DISCONNECT) {
            close(client);
            break;
        }

        // Reset Buffer
        memset(Buffer, 0, BUFFER_SIZE);
    }

    return NULL;
}
