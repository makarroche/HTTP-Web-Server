#include "func_Cadete.h"
#include "H_F_Child.h" //funcion verificar proceso HTTP

int continuar = 1;
char address_pm[256];
int port_pm;
int listening_port;

void capture_sig(int signal, siginfo_t *info, void *context) {

	int status;

	if(wait(&status) == -1){;
		SYSERROR(EXIT_FAILURE,"Error en el wait");
	}
	else{
		printf("Esta Entrando en Capture_Sig\n");
		continuar = 0;
		exit(EXIT_SUCCESS);
	}
}


int main (int argc, char **argv) {

	if (argc != 4) {
		printf("Cantidad de parametros inadecuada. Uso: ./Cadete <port> <address_pm> <port_pm>\n");
		exit(EXIT_FAILURE);
		SYSERROR(EXIT_FAILURE, "Cantidad de parametros inadecuada. Uso: ./Cadete <port> <address_pm> <port_pm>\n");
	}
	
	Request request_proccessing; 

	// para cortar el procesamiento
	set_signal(capture_sig, SIGINT);
	set_signal(capture_sig, SIGTERM);

	listening_port = atoi(argv[1]);
	strcpy(address_pm, argv[2]);
	port_pm = atoi(argv[3]);

	// bind del socket
	int sock_fd;
	sock_fd = sock_listen(listening_port);
	int socket;

	while (continuar > 0) {

		//void *msg_heap;
		char msg[MAX_LENGTH_DATA];
		//int msg_size;

		// aceptar conexiones desde afuera.
		printf("Cadete en escucha!: %d\n", listening_port);
		socket = sock_open(sock_fd);
		printf("Recibi la conexion del Handler_Frontend_Child.\n");

		// Leer el mensaje que llega desde el Handler_Frontend_Child
		socket_read(socket, msg);
		printf("Cadete recibio el request enviado desde Handler_Frontend_Child %s \n", msg);
			
		
		verificar_formato_HTTP (msg, &request_proccessing, 1);

		if (strcmp(msg, VERB_LIST) == 0) { 
			// muestra el esquema del sistema de archivos donde se esta corriendo.
			//handle_list(socket);

		} else if (strcmp(request_proccessing.method, VERB_GET) == 0 || strcmp(request_proccessing.method, "HEAD") == 0) {
			// recibe primero el verbo HTTP (GET, PUT, ...)
			// caso del GET, recibe el path en una segunda instancia.
			handle_get(socket, request_proccessing.url);
			

		} else if (strcmp(request_proccessing.method, VERB_PUT) == 0) {
			// para el PUT.
			// recibe el path y luego el contenido

            char request_aux[MAX_LENGTH_DATA];
            sprintf(request_aux, "%s %s HTTP/1.1 \\nHost: %s \\nContent-Length: %s \\nContent-Type: %s \\n\\n%s", VERB_SYNC_PUT,
                    request_proccessing.url, request_proccessing.host, request_proccessing.content_length, request_proccessing.content_type, request_proccessing.message_body);

            printf(" ========= \n");
            printf("%s",request_aux);
            printf(" ========= \n");
			handle_put(socket, request_proccessing.url, request_proccessing.message_body, request_aux, 1);

		} else if (strcmp(request_proccessing.method, VERB_DELETE) == 0) {
			// caso del DELETE, recibe el path en una segunda instancia.

            char request_aux[MAX_LENGTH_DATA];
            sprintf(request_aux, "%s %s HTTP/1.1 \\nHost: %s \\nContent-Length: %s \\nContent-Type: %s \\n\\n%s", VERB_SYNC_DELETE,
                    request_proccessing.url, request_proccessing.host, request_proccessing.content_length, request_proccessing.content_type, request_proccessing.message_body);
			handle_delete(socket, request_proccessing.url, request_aux, 1);

		} else if (strcmp(request_proccessing.method, VERB_SYNC_PUT) == 0) {
			// Se usa interno entre los cadetes para sincronizar

			handle_put(socket, request_proccessing.url, request_proccessing.message_body, msg, 0);

		} else if (strcmp(request_proccessing.method, VERB_SYNC_DELETE) == 0) {
			// Se usa interno entre los cadetes para sincronizar
			handle_delete(socket, request_proccessing.url, msg, 0);

		} else {
			socket_send(socket, "-1", 2);
		}

		// free(msg_heap);
		socket_close(socket);
	}
	
	exit(EXIT_SUCCESS);
}


int read_ok(int socket) {
	void *msg_heap_pm;
	char* msg_pm;
	int msg_size_pm;

	socket_recv(socket, &msg_heap_pm, &msg_size_pm);
	msg_pm = (char*) msg_heap_pm;
	msg_pm[msg_size_pm] = '\0';

	printf("Retorno |%s|\n", msg_pm);
	free(msg_heap_pm);

	return 1;
}

void handle_list(int socket) {

}

void handle_get(int socket, char file_path[256]) {
	void *msg_heap;
	char *msg;
	int msg_size;
	//char file_path[256];
	printf("Entro al GET 1\n");
	// leer el path.
	//socket_recv(socket, &msg_heap, &msg_size);
	printf("FILE PATH: %s\n", file_path);
	//msg = (char*)msg_heap;
	//msg[msg_size] = '\0';
	//sprintf(file_path, "%s", msg);
	//file_path[msg_size] = '\0';
	//free(msg_heap);
	printf("Entro al GET 2\n");
	// abre socket con el personal manager (puede ir por internet contemplando Backends Migrados)
	printf("ADRESS PM:%s y PORT PM:%d", address_pm, port_pm);
	int sock_pm = sock_connect (address_pm, port_pm);
	printf(" Connecto al Personal Manager! \n");
	
	
	socket_send(sock_pm, VERB_GET, strlen(VERB_GET));
	socket_send(sock_pm, file_path, strlen(file_path));
	printf("HIZO aqui\n");

	// este buffer es para el contenido del archivo
	socket_recv(sock_pm, &msg_heap, &msg_size);
	msg = (char*)msg_heap;
	msg[msg_size] = '\0';
	printf("Contenido del archivo que me mando Backend Personal Manager:%s\n", msg); 

	// Envia el contenido al Handler_Frontend_Child
	//socket_send(socket, msg, msg_size);
	socket_response(socket, msg);
	free(msg_heap);

	// cierra el socket contra el personal manager
	printf("HIZO TODO\n");
	socket_close(sock_pm);

}


void handle_put(int socket,  char file_path[256], char content[MAX_LENGTH_DATA], char request[MAX_LENGTH_DATA], int sync) {
//;
//
//void handle_delete(int socket, char file_path[256], int sync);
//void handle_put(int socket, int sync) {

	SHM_Access axs;

	printf("Entro al PUT 2\n");
	// abre socket con el personal manager (puede ir por internet contemplando Backends Migrados)
	printf("ADRESS PM:%s y PORT PM:%d", address_pm, port_pm);
	// abre un socket con el personal manager (puede ir por internet)
	int sock_pm = sock_connect (address_pm, port_pm);
	printf(" Connecto al personal manager! \n");

	socket_send(sock_pm, VERB_PUT, strlen(VERB_PUT));
	socket_send(sock_pm, file_path, strlen(file_path));
	socket_send(sock_pm, content, strlen(content));
	int status = read_ok(sock_pm);
	socket_close(sock_pm);

	if (status < 0) {
		printf("ERROR\n");
		socket_send(socket, "PUT -1", 6);
	} else {

		ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
		shm_be_mapping_and_semaphore_write_add_path (axs, file_path);
		detach_shared_memory(axs);

		// responde al FRON END
		socket_send(socket, "PUT OK", 6);

		// ahora sincroniza con los otros cadetes
		if (sync > 0) {
			Cadetes backs;

			ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
			shm_be_mapping_and_semaphore_read_ports(axs, &backs);
			detach_shared_memory(axs);

			for (int i = 0; i < backs.length; ++i) {
				int sync_port = backs.ids[i];

				// filtra que el puerto no sea el propio.
				if (sync_port != listening_port) {
					// abre un socket con el personal manager (puede ir por internet)
					printf(" Connecto al cadete %d! \n", sync_port);
					socket_client(request, NULL, sync_port);
				}
			}
		}
	}
}

void handle_delete(int socket, char file_path[256], char request[MAX_LENGTH_DATA], int sync) {

	SHM_Access axs;

	printf("Entro al PUT 2\n");
	// abre socket con el personal manager (puede ir por internet contemplando Backends Migrados)
	printf("ADRESS PM:%s y PORT PM:%d", address_pm, port_pm);

	// abre un socket con el personal manager (puede ir por internet)
	int sock_pm = sock_connect (address_pm, port_pm);
	printf(" Connecto al personal manager! \n");

	socket_send(sock_pm, VERB_DELETE, strlen(VERB_DELETE));
	socket_send(sock_pm, file_path, strlen(file_path));
	int status = read_ok(sock_pm);
	socket_close(sock_pm);

	if (status < 0) {
		printf("ERROR\n");
		socket_send(socket, "DELETE -1", 6);
	} else {

		ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
		shm_be_mapping_and_semaphore_write_del_path (axs, file_path);
		detach_shared_memory(axs);

		// responde al FRONT END
		socket_send(socket, "DELETE OK", 6);

		// Sincroniza con los otros cadetes
		if (sync > 0) {
			Cadetes backs;

			ask_shared_memory(&axs, TMPFILENAMEMAP, MYPROJECTIDMAP);
			shm_be_mapping_and_semaphore_read_ports(axs, &backs);
			detach_shared_memory(axs);

			for (int i = 0; i < backs.length; ++i) {
				int sync_port = backs.ids[i];

				// filtra que el puerto no sea el propio.
				if (sync_port != listening_port) {
					// abre un socket con el personal manager (puede ir por internet)
					printf(" Connecto al cadete %d! \n", sync_port);
					socket_client(request, NULL, sync_port);
				}
			}

		}
	}

}















	
