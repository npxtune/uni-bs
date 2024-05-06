
#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 5678

int init(int *server, struct sockaddr_in *address);

#endif //INIT_H