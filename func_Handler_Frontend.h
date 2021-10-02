#ifndef _FRONTEND_CHILD_
#define _FRONTEND_CHILD_

#include "includes.h"
#include "shared_memory.h"
#include "socket.h"

#define DAD_TO_KID 10
#define MYPROJECTKEY 73
#define _PORT_ 8080

//SIGCHLD Handler
void set_signal(void *handler, int signal_type);

//Capture SIGCHLD
void capture_sig_child(int signal, siginfo_t *info, void *context);

//Funcion para capturar APAGAR EL SISTEMA


#endif
