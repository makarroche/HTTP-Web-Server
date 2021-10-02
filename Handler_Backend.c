#include "func_Handler_Backend.h"


// Lista de los pids de los procesos hijos.
// PidsList pids_hijos;

int main (int argc, char **argv) {
	
	SHM_Access axs;	
		
	//Solicitar Shared Memory Mapping
	request_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
	initialize_shm_backend_mapping(axs);
	crearBEinicial(axs);
	detach_shared_memory(axs);
	
	int port_inicial = 6003;
	pid_t pid_hijo;
	//char buff_cmd[2048];

	set_signal(capture_sig_child, SIGCHLD);
	int continuar = 1;


	printf("Por comenzar escucha de puerto para la conexion del console manager.\n\n");
	sleep(10);
	// puerto para el socket de entrada (el que viene desde afuera)
	uint16_t port = BE_CONTROL_PORT;
	int sock_fd;
	int socket;

	sock_fd = sock_listen(port);
	socket = sock_open(sock_fd);
	//printf("Recibi la conexion del console manager.");
	
	int command;

	while ( continuar > 0 ) {

		//socket = sock_open(sock_fd);
		printf("Recibi la conexion del console manager.");

		command = leer_comando(socket);

		// ejecuta el comando que viene desde el console manager
		if (command == 0) {

			void* msg_heap = leer_addr_port(socket);
			char* msg = (char*) msg_heap;

			const char delimiter[2] = "|";
			char *token;

			// address donde esta el personal manager
			token = strtok(msg, delimiter);
			char address[256];
			strcpy(address, token);

			// puerto donde esta el personal manager
			token = strtok(NULL, delimiter);
			char port_str[10];
			strcpy(port_str, token);

			// libera la memoria del buffer que vino desde el recv.
			free(msg_heap);

			pid_hijo = agregarBE(port_inicial, address, port_str);


			ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
			shm_be_mapping_and_semaphore_write_add_be(axs, port_inicial);
			detach_shared_memory(axs);

			// agrega a la lista de pids el nuevo hijo.
			pids_hijos.pids[pids_hijos.length] = pid_hijo;
			pids_hijos.ids[pids_hijos.length] = port_inicial;
			pids_hijos.length++;

            char msg_buff[6] = "\0\0\0\0\0\0";
			sprintf(msg_buff, "%d", port_inicial);
           	socket_send(socket, msg_buff, strlen(msg_buff));

			port_inicial++;

		} else if (command == 1) {

			int be_a_borrar = leer_be_id(socket);
            int borro = 0;

			printf("Vamos a borrar el BE %d\n", be_a_borrar);
			// busca el hijo a borrar.
			for (int i = 0; i < pids_hijos.length; ++i) {

				// encontro el hijo a borrar
				if (pids_hijos.ids[i] == be_a_borrar) {
				    borro = 1;
					// me lo guardo para hacerle el kill
					pid_hijo = pids_hijos.pids[i];

					ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
					shm_be_mapping_and_semaphore_write_del_be(axs, pids_hijos.ids[i]);
					detach_shared_memory(axs);

					// ajusta la lista para que el ultimo pase al lugar del que se borra
					//   y luego resta uno en el largo total.
					pids_hijos.pids[i] = pids_hijos.pids[pids_hijos.length];
					pids_hijos.ids[i] = pids_hijos.ids[pids_hijos.length];
					pids_hijos.length--;

					// ahora le da el kill.
					kill(pid_hijo, SIGINT);

                	socket_send(socket, "1", 1);
					break;
				}
			}

			if (borro == 0) {
				socket_send(socket, "-1", 2);
			}

		} else if (command == 2) {

			continuar = 0;

			printf("Exiting server.. Goodbye\n");
			for (int i = 0; i < pids_hijos.length; ++i)
			{
				kill(pids_hijos.pids[i], SIGINT);
			}


		} else {
			printf(" No se entendio el mensaje recibido. \n");
		}
	}
	socket_close(socket);
    	exit(EXIT_SUCCESS);
}


