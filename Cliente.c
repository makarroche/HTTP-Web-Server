#include "includes.h"
#include "socket.h"
#define PORT_ClienteyH_F 8080
#define MAX_LENGTH_REQUEST 300

int main (int argc, char **argv) {
	char request[MAX_LENGTH_REQUEST];
	char msg[MAX_LENGTH_REQUEST];
	while(1) {
		
		printf("\n***********************************************************\n");
		printf("Request:\n");
		fgets(request, MAX_LENGTH_REQUEST, stdin);
		socket_client(request, msg, PORT_ClienteyH_F); // -> se mando el request
	}
}



//Para pruebas: CORRECTAS *****************************************************************************************

//GET /index.html HTTP/1.1 \nHost: localhost:8080
//HEAD /index.html HTTP/1.1 \nHost: localhost:8080
//DELETE /index.html HTTP/1.1 \nHost: localhost:8080
//PUT /index.html HTTP/1.1 \nHost: localhost:8080 \nContent-Length: 12 \nContent-Type: text/plain \n\nHello world! 
// PUT /archivo.txt HTTP/1.1 \nHost: localhost:8080 \nContent-Length: 12 \nContent-Type: text/plain \n\nHello world!

//Para pruebas: ERRORES EN FORMATO *********************************************************************************

//GET /index.html 
//TRACE /index.html HTTP/1.1 \nHost: localhost:8080
//get /index.html HTTP/1.1 \nHost: localhost:8080
//GET index.html HTTP/1.1 \nHost: localhost:8080
//GET /index.html HTTP/1.0 \nHost: localhost:8080
