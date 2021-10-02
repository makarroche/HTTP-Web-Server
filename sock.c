#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h> // Socket addresses
#include <arpa/inet.h> // Socket addresses
#include <string.h>
#include "sock.h"

/*

RECETA PARA TRABAJAR CON SOCKETS:

Pasos a seguir por el servidor:
	1- Crear el socket: socket().
	2- Llenar la estructura sockaddr correspondiente al la familia de protocolo
		 seleccionada:
			A- El path si es Unix Domain Socket.
			B- IP y puerto si es AF_INET.
	3- Nombrar al socket: bind().
	4- Establecer la cola de conexiones pendientes: listen().
 	5- Aceptar un a conexión: accept().
	6- Intercambio de mensajes con el cliente:
			- send() y/o write().
			- recv() y/o read().
	7- Cerrar la conexión: close() o shutdown() .

Pasos a seguir por un cliente:
	1- Crear el socket: socket().
	2- Llenar la estructura sockaddr correspondiente al la familia de protocolo seleccionada
		 con los datos del servidor:
			A- El path si es Unix Domain Socket.
			B- IP y puerto si es AF_INET.
	3- Establecer la conexión: connect().
	4- Intercambio de mensajes con el cliente:
			- send() y/o write().
			- recv() y/o read().
	5- Cerrar la conexión: close() o shutdown() .

*/

/*
  Funciones para manejo de Socets:
*/


/* Función para el servidor (1). */
int sock_listen (uint16_t port) {

	int sock;
	struct sockaddr_in name;

	/* 1- Crear el socket: socket(). */
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	/* 2- Llenar la estructura sockaddr correspondiente al la familia de protocolo
		 seleccionada:
			A- El path si es Unix Domain Socket.
			B- IP y puerto si es AF_INET.  */
	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl ( INADDR_ANY );

	/* 3- Nombrar al socket: bind(). */
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
		perror ("bind");
		exit (EXIT_FAILURE);
	}

	/* 4- Establecer la cola de conexiones pendientes: listen(). */
	if (listen (sock, MAX_CLIENTS) < 0) {
		perror ("listen");
		exit (EXIT_FAILURE);
	}

	return sock;
}

/* Función para el servidor (2). */
int sock_open (int sock) {

	/* 5- Aceptar un a conexión: accept(). */
	struct sockaddr addr;
	socklen_t length = sizeof(struct sockaddr);

	int rdsocket = accept (sock, &addr, &length);
	if (rdsocket < 0 ) {
		perror("accept");
		return -1;
	}
	return rdsocket;
}

/* Función para el cliente. */
int sock_connect (char* address, uint16_t port) {

	int sock;
	struct sockaddr_in servername;

	/* 1- Crear el socket: socket(). */
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror ("socket");
		exit (EXIT_FAILURE);
	}

	/* 2- Llenar la estructura sockaddr correspondiente al la familia de protocolo seleccionada
		 con los datos del servidor:
			A- El path si es Unix Domain Socket.
			B- IP y puerto si es AF_INET. */
	servername.sin_family = AF_INET;
	servername.sin_port = htons (port);
	if (address == NULL) {
		servername.sin_addr.s_addr = htonl ( INADDR_LOOPBACK );
	} else {
		if (inet_aton(address, &servername.sin_addr) == 0) {
			perror ("address name");
			exit (EXIT_FAILURE);
		}
	}

	/* 3- Establecer la conexión: connect(). */
	if (connect (sock, (struct sockaddr *) &servername, sizeof(servername)) == -1) {
		perror ("connect");
		exit (EXIT_FAILURE);
	}

	return sock;

}

/* Cerrar la conexión: close() o shutdown() . */
void socket_close (int fd) {
	/* Cierra socket. */
	shutdown(fd, SHUT_RDWR);
}


/* Intercambio de mensajes:
	La estructura del mensaje a enviar o recibir es:
					[INT con largo del dato][dato]
*/

/* Funcion para enviar el mensaje.
 Recibe los siguientes parámetros:
 - FD del socket.
 - El puntero al dato a enviar (void* dato) que debe haber sido generado
 antes de llamar a esta funcíon.
 - El tamaño del dato a enviar (dato_size) */
int socket_send (int fd, void *dato, int dato_size) {
	void *msg;
	int sent;

	/* Genera el mensaje en el Heap. */
	msg = malloc(sizeof(int)+dato_size);
	memcpy(msg, &dato_size, sizeof(int));
	memcpy(msg+sizeof(int), dato, (size_t)dato_size);

	/* Envia mensaje. */
	if( (sent = send(fd, msg, sizeof(int) + ((size_t)dato_size), 0)) < 0 ) {
		perror ("send");
		exit (EXIT_FAILURE);
	}
	/* Libero memoria. */
	free(msg);

	/* Si se envió todo devuelve 0. */
	return dato_size - sent + sizeof(int);
}

/* Funcion para recibir el mensaje.
 Recibe los siguientes parámetros:
 - FD del socket.
 - Un puntero a un puntero (void **dato) donde la función apuntará a un espacio en el heap
 con el dato recibido.
 - Un puntero a un int donde la función guardará el tamaño del dato recibido
 */
int socket_recv (int fd,  void **dato, int *dato_size) {
	int size, recvd;
	void *msg;

	/* Genera espacio en el Heap para el dato. */
	msg = malloc(MAX_DAT_SIZE);

	/* Lee largo del mensaje y lo guarda en dato_size */
	if ( (recvd = recv(fd, (void *) &size, sizeof(int), 0)) < sizeof(int) ) {
//printf(">>>>> RECIBIDO: %d\n",recvd);
		perror ("recv size");
		exit (EXIT_FAILURE);
	}
	
	memcpy(dato_size, &size, sizeof(int));

	/* Lee mensaje y lo guarda en dato */
	if ( (recvd = recv(fd, msg, size, 0)) < 0 ) {
		perror ("recv dato");
		exit (EXIT_FAILURE);
		
	}
	
	memcpy(dato, &msg, sizeof(void *));
	/* Si se recibió todo lo esperado devuelve 0. */
	return 	size - recvd;
}
