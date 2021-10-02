#include "func_consola.h"

int main(int argc, char **argv){
	

	while(TRUEE){

		char option;
		int check_sf;

		printf(" \n Bienvenido a la consola de administracion\n             Menu Principal\n");
		printf(" 1.Reporte de Input/Output y pedidos efectivos con los clientes\n"); 
		printf(" 2.Reporte de carga de los Backends\n"); 
		printf(" 3.Agregar Backend\n");
		printf(" 4.Quitar Backend\n");
		printf(" 0.Exit\n\n");
		check_sf = scanf("%c", &option);

		if(OPCION_MENU_0  <= option && option <=  OPCION_MENU_4 && check_sf == CANT_CHAR_SCANF){ 
			
			printf("*****************************************************************************\n"); 
			if(option == OPCION_MENU_1){
				socket_client("Opcion_1", NULL, PORT_CyCM);
			}
			
			if(option == OPCION_MENU_2){
				socket_client("Opcion_2", NULL, PORT_CyCM);

			}
			
			if(option == OPCION_MENU_3){
				
				/*printf("Ingrese el URL del Backend a agregar\n");
				char buff[25];
				char port[10];
				printf("Ingrese IP server: \n");
				scanf("%s", buff);
				printf("\n");
                		printf("Ingrese puerto: \n");
				scanf("%s", port);
               			printf("\n");

				printf("Agregando backend - Personal manager %s:%s\n", buff, port);
				int sock = sock_connect("127.0.0.1", CM_CONTROL_PORT);
				socket_send(sock, "3", 1);
				char command[35];
				sprintf(command, "%s|%s", buff, port);
				socket_send(sock,command, strlen(command));

				void *msg_heap;
				char *msg;
				int msg_size;
				socket_recv(sock, &msg_heap, &msg_size);
				msg = (char *) msg_heap;
				msg[msg_size] = '\0';

				//Mostrar respuesta	
				printf("Backend %s agregado exitosamente al sistema\n", msg);
				free(msg_heap);*/
				
			}
	
			if(option == OPCION_MENU_4){
				//socket_client("Opcion_4", URL);
				//printf("Ingrese el URL del Backend a eliminar\n");
				//fgets(URL, MAX_PATH_SIZE, stdin);
				socket_client("Opcion_4", NULL, PORT_CyCM);
				printf("Backend (Identificador de Backend) eliminado exitosamente al sistema\n");
			}
			
			if(option == OPCION_MENU_0){
				printf("Cerrando Consola de Administracion...Apagando el Sistema...Hasta Luego\n");
				socket_client("Opcion_4", "Shutdown", PORT_CyCM);
				exit(EXIT_SUCCESS);			
			
			}
			getchar(); // Elimina el salto de linea luego de una opcion correcta marcada
			printf("*****************************************************************************\n\n");
		}
		else{
			if(check_sf != CANT_CHAR_SCANF){
				SYSERROR(EXIT_FAILURE, "\nError interno... Volver a intentar\n");
			}
	
			if(option == ENTER || option == SPACE ){
				printf("No se ingreso una opcion\n");
			}

			else{
				printf("\nLa opcion ingresada no es correcta\n");
				getchar();
			}
					
		}
	}
} 


