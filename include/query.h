
#ifndef QUERY_H
#define QUERY_H
#include "definitions.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

int query(const client_data *data, const int *client);

int exec(int commandIndex, const client_data *data, const int *client);

#endif //QUERY_H
