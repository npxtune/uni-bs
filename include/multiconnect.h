
#ifndef MULTICONNECT_H
#define MULTICONNECT_H

#include <netinet/in.h>

void multiconnect(const int *server, struct sockaddr_in *address);
void *connected_client(void *arg);
void disconnect_all_clients();

#endif //MULTICONNECT_H
