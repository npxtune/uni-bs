
#ifndef QUERY_H
#define QUERY_H
#include "definitions.h"
#include "data_storage.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

// Enum for command strings
enum command_strings { get_command, put_command, del_command, dc_command, quit_command, beg_command, end_command, sub_command };

// Function prototypes
int query(const client_data *data, const int *client);
int exec(const int commandIndex, const client_data *data, const int *client);

#endif // QUERY_H


