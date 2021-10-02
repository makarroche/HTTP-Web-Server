#include "H_F_Child.h"

int main (int argc, char **argv) {

	char request[MAX_LENGTH_REQUEST]; 
	char response[MAX_LENGTH_RESPONSE];
	char request_aux[MAX_REQUEST_AUX];
	char message_body[MAX_LENGTH_DATA];
	char direccion_Backend[MAX_LENGTH_REQUEST];
	int result;
	int puerto_cadete_menor_carga_be;
	int content_length;
	Request req;
	SHM_Access axs;
	

	// Preguntar Shared Memory Management Existente
	ask_shared_memory(&axs, "shm_management", MYPROJECTKEY);
	
	// Le derivan la conexion
	printf("\n*Estoy en el hijo*\n");
	int me_derivaron_conexion  = atoi(argv[1]);	

	// leer el request del cliente desde el socket
	socket_read(me_derivaron_conexion, request);
	 
	printf("\nRequest:%s\n",request);
	
	// Revisar formato http
	result = verificar_formato_HTTP (request, &req, 0);
	if(result == FORMAT_OK){
		printf("\n***************************\n");
		printf("\nFormato Correcto\n");
		printf("Request para HTTP Management: %s %s %s %s %s %s %s\n" , req.method, req.url, req.host, 				       			req.content_length, req.content_type, req.message_body, req.request_status);
		printf("\n***************************\n");
		
		// Funcion Entrar a Shared Memory Management; Modificar Input Efectivo #Solicitudes Efectivas
		shm_management_and_semaphore_write(axs, "solicitud_efectiva");
		
		// Mandar Request a HTTP Management por el socket
		if(strcmp(req.method, "PUT") == 0){
			sprintf(request_aux, "%s %s HTTP/1.1 \\nHost: %s \\nContent-Length: %s \\nContent-Type: %s \\n\\n%s", req.method, 				req.url, req.host, req.content_length, req.content_type, req.message_body);
			printf("Request Reconstruida:\n%s",request_aux);

			// Va el Request para HTTP Management y Recibir direcciones de Backends de HTTP Management
			socket_unix_client(request_aux, direccion_Backend, UNIX_DOMAIN_SOCKET_PATH_HTTPMYHFC); 

			//Traducir Puerto
			puerto_cadete_menor_carga_be = atoi(direccion_Backend);

			// Conexion a Cadete con la informacion del Request 
			socket_client(request_aux, NULL, puerto_cadete_menor_carga_be);
			
			printf("Request Enviada al Cadete:%s\n", request_aux);
			// Esperar a que respuesta del Cadete
			sleep(2); 
			
			// Funcion Entrar a Shared Memory Management; Modificar #Respuestas Efectivas
			shm_management_and_semaphore_write(axs,"respuesta_efectiva");
			sprintf(response, "HTTP/1.1 201 Created\nContent-Type: text/plain\nContent-Length: %s\n\nHTTP/1.1 201 Created", 			req.content_length);
			socket_response(me_derivaron_conexion, response);
		}
		else{ //Es GET o HEAD o DELETE
			sprintf(request_aux,"%s %s HTTP/1.1 \\nHost: %s", req.method, req.url, req.host);
			printf("Request Reconstruida:\n%s",request_aux);
			
			// Va el Request para HTTP Management y Recibir direccion de Backend de Menor Carga
			socket_unix_client(request_aux, direccion_Backend, UNIX_DOMAIN_SOCKET_PATH_HTTPMYHFC);
			//Buscar el recurso

			if(1){ //TODO BUSCAR SI EL RECURSO ESTA!!!!
				
				if(strcmp(req.method,"GET") == 0 || strcmp(req.method,"HEAD") == 0 ){
					//Traducir Puerto
					puerto_cadete_menor_carga_be = atoi(direccion_Backend);
					printf("Handler Frontend Child accede a puerto de Cadete: %d ", puerto_cadete_menor_carga_be);
					
					// Conexion a Cadete con la informacion del Request 
					socket_client(request_aux, message_body, puerto_cadete_menor_carga_be);
					printf("Request Enviada al Cadete:%s por el puerto:%d\n", request_aux, puerto_cadete_menor_carga_be);
					// Esperar a que respuesta del Cadete
					content_length = strlen(message_body); 			

					if(strcmp(req.method,"GET") == 0){ //Es HEAD Solo encabezados del recurso
						sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: %d \n\n%s", content_length, message_body); 
					
					}
					else{ //Es HEAD Solo encabezados del recurso
						sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: %d \n\n", content_length); 	
					}
					socket_response(me_derivaron_conexion, response); //Respuesta al Cliente
				}
				else{ //Es DELETE 
					//Traducir Puerto
					puerto_cadete_menor_carga_be = atoi(direccion_Backend);

					// Conexion a Cadete con la informacion del Request 
					socket_client(request_aux, NULL, puerto_cadete_menor_carga_be);
					printf("Request Enviada al Cadete:%s\n", request_aux);
					//Esperar Respuesta del Cadete
					sleep(2); 

					//Respuesta al Cliente
					socket_response(me_derivaron_conexion, 
					"HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 16\n\nResource Deleted");
				}
				
				// Funcion Entrar a Shared Memory Management; Modificar #Respuestas Efectivas
				shm_management_and_semaphore_write(axs, "respuesta_efectiva");
			}
			else{
				//Respuesta al Cliente
				socket_response(me_derivaron_conexion, 
				"HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 22\n\nHTTP/1.1 404 Not Found");
				// Funcion Entrar a Shared Memory Management; Modificar #Respuestas con error
				shm_management_and_semaphore_write(axs, "respuesta_error");
			}
		}
	}
	else{

		if(result == NOT_IMPLEMENTED || result == BAD_REQUEST || result == NOT_SUPPORTED){
			printf("STOP! el formato es incorrecto\n");
			if(result == NOT_IMPLEMENTED){
				strcpy(response, 
				"HTTP/1.1 501 Not Implemented\nContent-Type: text/plain\nContent-Length: 28\n\nHTTP/1.1 501 Not Implemented");
				printf("%s\n", response);
			}
			if(result == BAD_REQUEST){
				strcpy(response, 
				"HTTP/1.1 400 Bad Request\nContent-Type: text/plain\nContent-Length: 24\n\nHTTP/1.1 400 Bad Request");
				printf("%s\n", response);
			}
			if(result == NOT_SUPPORTED){
				strcpy(response, 
				"HTTP/1.1 505 Version Not Supported\nContent-Type: text/plain\nContent-Length: 34\n\nHTTP/1.1 505 Version Not Supported");
				printf("%s\n", response);
			}
			
			// Funcion Entrar a Shared Memory Management; Modificar Input Efectivo #Solicitudes Rechazadas
			shm_management_and_semaphore_write(axs, "solicitud_rechazada");

			//Hubo error de formato, respuesta indmediata al Cliente
			socket_response(me_derivaron_conexion, response);
		}
			
	}
	// Detachear el proceso de Shared Memory Management
	detach_shared_memory(axs);
	// Cerrar conexion con el Cliente
	close_socket(me_derivaron_conexion);
	// Die 
	exit(EXIT_SUCCESS);
}




