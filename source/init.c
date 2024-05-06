#include "init.h"

const int OPT = 1;

int init(int *server, struct sockaddr_in *address) {
    // Erstellt einen Socket
    if ((*server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setzt Optionen für den Socket
    if (setsockopt(*server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &OPT, sizeof(OPT))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Konfiguriert Adressenstruktur
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    // Bindet den Socket an die Adresse
    if (bind(*server, (struct sockaddr*)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Hört auf eingehende Verbindungen
    if (listen(*server, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
