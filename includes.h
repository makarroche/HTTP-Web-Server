#ifndef _INCLUDES_
#define _INCLUDES_


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <ctype.h>
//****************
#include <sys/un.h>
 

//shared memory
#include <sys/ipc.h>
#include <sys/shm.h>

//sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//senales
#include <signal.h>
#include <sys/wait.h>

//#include "shared_memory.h"
#include "Estructuras.h"
#include "semaphore.h"


#define SYSERROR(status, msg) error_at_line(status, errno, __FILE__, __LINE__, msg)

#endif
