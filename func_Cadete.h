#ifndef _CADETE_
#define _CADETE_

#include "includes.h"
#include "sock.h"
#include "socket.h"
#include "shared_memory.h"

#define MYPROJECTIDMAP 55

void capture_sig(int signal, siginfo_t *info, void *context);

void set_signal(void *handler, int signal_type);

void handle_list(int socket);

void handle_get(int socket, char file_path[256]);

void handle_put(int socket,  char file_path[256],  char content[MAX_LENGTH_DATA], char request[MAX_LENGTH_DATA], int sync);

void handle_delete(int socket, char file_path[256], char request[MAX_LENGTH_DATA], int sync);

int read_ok(int socket);


#endif
