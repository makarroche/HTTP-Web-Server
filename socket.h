#ifndef _SOCKETS_
#define _SOCKETS_

#include "includes.h"

#define MAX_SERVER_RESPONSE 5000
#define MAX_LENGTH_QUERY 200
#define MAX_LENGTH_DATA 5000
//******
#define SOCKET_FILENAME_MAX_SIZE 50
#define UNIX_DOMAIN_SOCKET_PATH_HTTPMYHFC "unix_socket_http_management_handler_frontend_child"
#define UNIX_DOMAIN_SOCKET_PATH_HFCYC "unix_socket_client_handler_frontend_child_cadete"

//INTERNET DOMAIN SOCKETS*****************************************

//Crea el socket
Socket_Access create_socket(int port_socket);

//Socket escuchando
int socket_escucha(Socket_Access socket_axs);

//Lectura de lo enviado por el cliente por socket 
void socket_read(int new_socket, char *read_to_here);

//Escritura al Client por el socket
void socket_response(int new_socket, char data_for_consola[MAX_SERVER_RESPONSE]);

//Cliente pidiendo conexion por socket
int socket_client(char request_sock[MAX_LENGTH_QUERY], char data[MAX_LENGTH_DATA], int port_socket);

//Cierra el socket
void close_socket(int new_socket);

//UNIX DOMAIN SOCKETS**********************************************

//Crea el socket
Socket_Access_Unix create_unix_socket(char socket_filename[SOCKET_FILENAME_MAX_SIZE]);

//Socket escuchando
int socket_unix_escucha(Socket_Access_Unix socket_axs_unix);

//Lectura de lo enviado por el cliente por socket 
void socket_unix_read(int new_socket, char *read_to_here);

//Escritura al Client por el socket
void socket_unix_response(int new_socket, char data_for_consola[MAX_SERVER_RESPONSE]);

//Escritura al Client por el socket
int socket_unix_client(char request_sock[MAX_LENGTH_QUERY], char *data, char socket_filename[SOCKET_FILENAME_MAX_SIZE]);

//Cierra el socket
void close_unix_socket(int new_socket);








#endif
