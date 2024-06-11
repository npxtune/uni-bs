#include "multiconnect.h"

#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <query.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "definitions.h"

bool ShutdownRequested = false;
char Buffer[BUFFER_SIZE] = {0};
int ClientSockets[MAX_CLIENTS];
int NumClients = 0;


void multiconnect(const int *server, struct sockaddr_in *address) {
    while (!ShutdownRequested) {
        // Prepare file descriptors for select
        fd_set readFds;
        FD_ZERO(&readFds);
        FD_SET(*server, &readFds);

        // Set timeout to check for shutdown periodically
        struct timeval timeout;
        timeout.tv_sec = 1; // 1 second
        timeout.tv_usec = 0;

        const int activity = select(*server + 1, &readFds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("Error in select");
            break;
        }

        if (FD_ISSET(*server, &readFds)) {
            socklen_t length = sizeof(address);
            int *client = malloc(sizeof(int)); // Allocate memory for client socket descriptor
            if ((*client = accept(*server, (struct sockaddr*)address, &length)) < 0) {
                perror("Accept failed");
                free(client); // Free allocated memory before exiting
                continue;
            }

            // Add client socket to array
            ClientSockets[NumClients++] = *client;

            // Create a thread to handle client requests
            pthread_t tid;
            pthread_create(&tid, NULL, connected_client, (void *)client);
            pthread_detach(tid); // Detach the thread to avoid memory leaks
        }
    }

    // Disconnect all clients before exiting
    disconnect_all_clients();
}

void *connected_client(void *arg) {
    const int client = *(int*)arg; // Cast void pointer back to int pointer and dereference
    free(arg); // Free memory allocated for argument

    // Read input from client
    while (!ShutdownRequested) {
        const int valread = read(client, Buffer, BUFFER_SIZE);

        for (int i = 0; i < sizeof(Buffer); i++) {
            // Make non case sensitive
            Buffer[i] = toupper(Buffer[i]);
        }
        Buffer[valread] = '\0'; // NULL char

        printf("Client %d sent command: %s", client, Buffer); // For debugging :)

        const client_data data = {strtok(Buffer, " "), strtok(NULL, " "), strtok(NULL, " ")};

        const int result = query(&data, &client);

        if (result == CLIENT_DISCONNECT) {
            for (int i = 0; i < NumClients; i++) {
                if (ClientSockets[i] == client) {
                    ClientSockets[i] = ClientSockets[i+1];
                    NumClients -=1;
                    break;
                }
            }
            close(client);
            break;
        }
        if (result == SERVER_SHUTDOWN) {
            ShutdownRequested = true;
            return NULL;
        }

        // Reset Buffer
        memset(Buffer, 0, BUFFER_SIZE);
    }
    return NULL;
}

void disconnect_all_clients() {
    for (int i = 0; i < NumClients; i++) {
        close(ClientSockets[i]);
    }
    NumClients = 0; // Reset number of clients
}