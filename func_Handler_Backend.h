#ifndef _HANDLER_BACKEND_
#define _HANDLER_BACKEND_

#include "includes.h"
#include "sock.h"
#include "shared_memory.h"
#include "func_Handler_Frontend.h" //Incluye los Handlers de SIGCHLD

#define MYPROJECTIDMAP 55

PidsList pids_hijos;

//Inizializa el sistema creanado 3 backends
void  crearBEinicial();

//Funcion agregar BE 
pid_t agregarBE(int listen_port, char address[], char port_str[]);

void registrarBEenMapping(SHM_Access axs, int id);

void eliminarBEenMapping(SHM_Access axs, int id);

int leer_comando(int socket);

void* leer_addr_port(int socket);

int leer_be_id(int socket);



#endif
