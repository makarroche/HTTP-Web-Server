#include "func_Handler_Backend.h"

void crearBEinicial(SHM_Access axs){	

	pid_t pid_hijo;

	// agrega el BE
	pid_hijo = agregarBE(6000, "127.0.0.1", "7000");

	// persiste el pid en la lista
	pids_hijos.pids[0] = pid_hijo;
	pids_hijos.ids[0] = 6000;

	//registra el BE en el mapping
	shm_be_mapping_and_semaphore_write_add_be(axs, 6000);

	pid_hijo = agregarBE(6001, "127.0.0.1", "7001");
	pids_hijos.pids[1] = pid_hijo;
	pids_hijos.ids[1] = 6001;
	shm_be_mapping_and_semaphore_write_add_be(axs, 6001);

	pid_hijo = agregarBE(6002, "127.0.0.1", "7002");
	pids_hijos.pids[2] = pid_hijo;
	pids_hijos.ids[2] = 6002;
	shm_be_mapping_and_semaphore_write_add_be(axs, 6002);

	pids_hijos.length = 3;
	printf(" Termina inicializacion de los BE. \n");
}


int leer_comando(int socket) {

	char* msg;
	void *msg_heap;
	int msg_size;
	int cmd;

	if (socket_recv(socket, &msg_heap, &msg_size) < 0) {
		SYSERROR(-1, "ERROR EN SOCKET!\n");
		exit(EXIT_FAILURE);
	}

	msg = (char*)msg_heap;

	if (strcmp(msg, CMD_AGREGAR) == 0) {
		cmd = 0;
	} else if (strcmp(msg, CMD_QUITAR) == 0) {
		cmd = 1;
	} else if (strcmp(msg, CMD_SALIR) == 0) {
		cmd = 2;
	} else {
		cmd = -1;
	}

	// libera la memoria que se pide adentro del socket_recv.
	free(msg_heap);
	return cmd;
}


void* leer_addr_port(int socket) {
	void *msg_heap;
	int msg_size;


	if (socket_recv(socket, &msg_heap, &msg_size) < 0) {
		SYSERROR(-1, "ERROR EN SOCKET!\n");
		exit(EXIT_FAILURE);
	}

	return msg_heap;
}

int leer_be_id(int socket) {
	char* msg;
	void *msg_heap;
	int msg_size;
	int be_id;

	if (socket_recv(socket, &msg_heap, &msg_size) < 0) {
		SYSERROR(-1, "ERROR EN SOCKET!\n");
		exit(EXIT_FAILURE);
	}

	msg = (char*)msg_heap;

	be_id = atoi(msg);

	free(msg_heap);
	return be_id;
}




//Funcion agregar backend 

pid_t agregarBE(int listen_port, char address[], char port_str[]){
	printf("Vamos a agregar el BE %d - %s:%s\n", listen_port, address, port_str);

	char port[8];
	sprintf(port, "%d", listen_port);
	pid_t resultado_forkAgregarBE;

	//Funcion crear proceso Cadete
	if ((resultado_forkAgregarBE = fork()) > 0) {
		// Estoy en el proceso padre
		//Crea un Cadete y un nuevo BE

		return resultado_forkAgregarBE;
	} else if (resultado_forkAgregarBE == 0) {
		// Estoy en el proceso hijo
		if (execl("./Cadete", "./Cadete", port, address, port_str, NULL) == -1) {
			SYSERROR(EXIT_FAILURE, "Error al ejecutar el Cadete.\n");
		}

		// el hijo corrio en forma exitosa.
		exit(EXIT_SUCCESS);
	} else {
		SYSERROR(EXIT_FAILURE, "Error al generar proceso hijo Cadete.");
	}	

	return -1;
}


