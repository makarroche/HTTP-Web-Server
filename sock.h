#ifndef _SOCK_H_
#define _SOCK_H_

#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 1
#define PORT 3045
#define SERVERHOST "127.0.0.1"
#define MAX_DAT_SIZE 1024

/*
  Funciones para manejo de Sockets:
*/

/* Función para el servidor (1). */
int sock_listen (uint16_t port);

/* Función para el servidor (2). */
int sock_open (int sock);

/* . */
int sock_connect (char* address, uint16_t port);

/* Cerrar la conexión: close() o shutdown() . */
void socket_close (int fd);

/*

Intercambio de mensajes:
	La estructura del mensaje a enviar o recibir es:
					[INT con largo del dato][dato]
*/

/* Funcion para enviar mensaje.
 Recibe los siguientes parámetros:
 - FD del socket.
 - El puntero al dato a enviar (void* dato) que debe haber sido generado
 antes de llamar a esta funcíon.
 - El tamaño del dato a enviar (dato_size) */
int socket_send (int fd, void *dato, int dato_size);

/* Funcion para recibir mensaje.
 Recibe los siguientes parámetros:
 - FD del socket.
 - Un puntero (void* dato) donde la función apuntará a un espacio en el heap
 con el dato recibido.
 - Un puntero a un int donde la función guardará el tamaño del dato recibido
 */
int socket_recv (int fd,  void **dato, int *dato_size);

#endif
