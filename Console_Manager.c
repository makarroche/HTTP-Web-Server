#include "func_Console_Manager.h"
#define MYPROJECTIDMAP 55

int main (int argc, char **argv) {

	char opcionConsola [20];
	int Input_Neto;
	int Output_Neto;	
	Socket_Access socket_axs;
	int new_socket;
	char *backends_carga = '\0';
	SHM_Backend_Mapping bemap;

	// Preguntar por Shared Memorys
	SHM_Access axs_shm_mngt, axs_shm_mapping;
	SHM_Management shm_result;

	ask_shared_memory(&axs_shm_mngt, "shm_management", MYPROJECTKEY);
	ask_shared_memory(&axs_shm_mapping,"shm_mapping", MYPROJECTIDMAP);
	
	// Mientras no venga la senal para matar el proceso atiende requests
	
	// Crea el socket con Consola
	socket_axs = create_socket(PORT_CyCM);

	while(1){
		char buffer_data[30000] = {0};

		//Socket en escucha con Consola
		new_socket = socket_escucha(socket_axs); 
		socket_read(new_socket, opcionConsola);
		//printf("Opcion Consola: %s\n", opcionConsola);
		if(strcmp(opcionConsola, "Opcion_1") == 0){
			printf("Opcion Consola: %s\n", opcionConsola);
			
			//Consultar Shared Memory Management; 
			shm_result = shm_management_and_semaphore_read(axs_shm_mngt);
			Input_Neto = shm_result.solicitudes_efectivas + shm_result.solicitudes_rechazadas;
			Output_Neto = shm_result.respuestas_efectivas + shm_result.respuestas_con_error;
			printf("Opcion Consola: %d\n", Input_Neto );
			sprintf(buffer_data, "Reporte de Input/Output\n\nInput Neto:%d\nInput Efectivo:%d\n"
			"Output Neto :%d\nOutput Efectivo:%d\n\n\nPedidos efectivos Cliente - Sistema:%d\n",Input_Neto, 			shm_result.solicitudes_efectivas, Output_Neto,shm_result.respuestas_efectivas, shm_result.respuestas_efectivas);
			socket_response(new_socket, buffer_data);
		}
		if(strcmp(opcionConsola, "Opcion_2") == 0){
			printf("Opcion Consola: %s\n", opcionConsola);
			
			//Consultar Shared Memory Backend Mapping
			mostrar_carga_backends(&bemap, backends_carga);
			printf("Sale \n");
			socket_response(new_socket, backends_carga);
		}
		if(strcmp(opcionConsola, "Opcion_3") == 0){
				/*
			    socket_recv(socket, &msg_heap, &msg_size);
			    msg = (char*)msg_heap;
			    msg[msg_size] = '\0';

			    int sock = sock_connect ("127.0.0.1", BE_CONTROL_PORT );
			    printf(" Connecto con BE padre! \n");

			    socket_send(sock, "A", 1);
			    socket_send(sock, msg, strlen(msg));
			    free(msg_heap);

			    // lee la respuesta del backend padre
			    socket_recv(sock, &msg_heap, &msg_size);
			    msg = (char*)msg_heap;
			    msg[msg_size] = '\0';

			    // envia a consola
			    socket_send(socket, msg, strlen(msg));
			    free(msg_heap);

			    socket_close(sock);*/
		}
		if(strcmp(opcionConsola, "Opcion_4") == 0){
		    /*
		    socket_recv(socket, &msg_heap, &msg_size);
		    msg = (char*)msg_heap;
		    msg[msg_size] = '\0';

		    int sock = sock_connect ("127.0.0.1", BE_CONTROL_PORT );
		    printf(" Connecto con BE padre! \n");

		    socket_send(sock, "A", 1);
		    socket_send(sock, msg, strlen(msg));
		    free(msg_heap);

		    // lee la respuesta del backend padre
		    socket_recv(sock, &msg_heap, &msg_size);
		    msg = (char*)msg_heap;
		    msg[msg_size] = '\0';

		    // envia a consola
		    socket_send(socket, msg, strlen(msg));
		    free(msg_heap);

		    socket_close(sock);*/

		}
		if(strcmp(opcionConsola, "Opcion_0") == 0){
			printf("Opcion 0 Shutdown!\n");
			/*int sock = sock_connect ("127.0.0.1", BE_CONTROL_PORT );
            		printf(" Connecto con BE padre! \n");
            		socket_send(sock, "S", 1);
            		socket_close(sock);*/
			pid_t pid = getpid();
			kill(pid, SIGKILL); //SIGTERM
			detach_shared_memory(axs_shm_mngt); 
			detach_shared_memory(axs_shm_mapping);
			finish_shared_memory(axs_shm_mngt, "shm_management"); 
			finish_shared_memory(axs_shm_mapping, "shm_mapping"); 
			//send murder in socket	
			exit(EXIT_SUCCESS);
		}
		//close_socket(new_socket); 
	}
}

