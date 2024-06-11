#include "init.h"
#include "multiconnect.h"
#include "query.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

bool IsClientConnected = false;
int Server;
struct sockaddr_in Address;


int main() {
    if (init(&Server, &Address) == EXIT_SUCCESS) {
        // Initialize Socket
        printf("Opened Socket on Port: %d\n", PORT);
    }
    else {
        perror("FAILED TO OPEN SOCKET!\n");
        return -1;
    }

    multiconnect(&Server, &Address);

    close(Server);
    return EXIT_SUCCESS;
}