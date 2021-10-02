#include "includes.h"
#include "shared_memory.h"
#include "socket.h"
#include "H_F_Child.h" 

#define LENGTH_RESPONSE_DIRECCIONES 1000
#define YOURPROJECTKEY 55
#define PUERTO_CADETE_BE_CON_MENOR_CARGA 50

int main (int argc, char **argv) {
	
	//char response[MAX_LENGTH_RESPONSE];
	int new_socket;	
	char request_in_queue[MAX_LENGTH_REQUEST];
	int backend_de_menor_carga;
	char puerto_cadete[PUERTO_CADETE_BE_CON_MENOR_CARGA];
	
	SHM_Access axs_shm_mapping;
	Socket_Access_Unix axs_unix_socket;
	Request request_proccessing; 

	//Preguntar Shared Memory Backend Mapping
	ask_shared_memory(&axs_shm_mapping, "shm_mapping", YOURPROJECTKEY); 

	// Crea socket con Handler_Frontend_Child
	axs_unix_socket = create_unix_socket(UNIX_DOMAIN_SOCKET_PATH_HTTPMYHFC);
	
	// Mientras no venga la senal para matar el proceso atiende requests
	while(1){
		// En escucha con Handler_Frontend_Child
		new_socket = socket_unix_escucha(axs_unix_socket);
		socket_unix_read(new_socket, request_in_queue);
		printf("\nRequest In Queue:%s\n", request_in_queue);
		verificar_formato_HTTP (request_in_queue, &request_proccessing, 0);
	
		//Procesar siguiente Request en cola
		if((strcmp(request_proccessing.method, "GET") == 0 || strcmp(request_proccessing.method, "HEAD") == 0 
		|| strcmp(request_proccessing.method, "DELETE") == 0 )){
//TODO Busco el recurso en un Backend -> si esta en uno, esta en todos. ojo con requests concurrentes
//TODO req.url buscarlo en Mapping!!!!!!!!!!!

			backend_de_menor_carga = shm_be_mapping_and_semaphore_write_menor_be(axs_shm_mapping);
			printf("Desde http_management puerto de Backend con menos carga: %d", backend_de_menor_carga);
			sprintf(puerto_cadete, "%d", backend_de_menor_carga);
		}
		else{
			if((strcmp(request_proccessing.method, "PUT")) == 0){
				backend_de_menor_carga = shm_be_mapping_and_semaphore_write_menor_be(axs_shm_mapping);
				sprintf(puerto_cadete, "%d", backend_de_menor_carga);
			}
		}
		socket_unix_response(new_socket, puerto_cadete);
	}
	
	//Llego señal para matar todos los procesos

	//Detach el proceso de Shared Memory Management
	detach_shared_memory(axs_shm_mapping);
	//Cerrar Socket con Handler_Frontend_Child
	close_unix_socket(new_socket);
	//Die
	exit(EXIT_SUCCESS);
}

