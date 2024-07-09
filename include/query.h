
#ifndef QUERY_H
#define QUERY_H
#include "definitions.h"
#include "data_storage.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


// Function prototypes
int query(const client_data *data, const int *client);
int exec(const int commandIndex, const client_data *data, const int *client);
int handle_get(const client_data *data, const int *client);
int handle_put(const client_data *data, const int *client);
int handle_del(const client_data *data, const int *client);
int handle_dc(const int *client);
int handle_quit(const int *client);
int handle_beg(const int *client);
int handle_end(const int *client);
int handle_sub(const client_data *data, const int *client);

#endif // QUERY_H


