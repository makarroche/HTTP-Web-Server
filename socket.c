#include "socket.h"

//Internet Domain Sockets

//Crea Socket
Socket_Access create_socket(int socket_port){ 

	    int server_fd;
	    struct sockaddr_in address;
	    Socket_Access sock_axs;
	    
	    // Creating socket file descriptor
	    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
		perror("In socket");
		exit(EXIT_FAILURE);
	    }
	    
	    address.sin_family = AF_INET;
	    address.sin_addr.s_addr = INADDR_ANY;
	    address.sin_port = htons(socket_port);
	    
	    memset(address.sin_zero, '\0', sizeof address.sin_zero);
	    
	    
	    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
		perror("In bind");
		exit(EXIT_FAILURE);
	    }
	    if (listen(server_fd, 10) < 0){ //Maximo numero de conexiones en cola, despues se rechazan
		perror("In listen");
		exit(EXIT_FAILURE);
	    }
		sock_axs.fd_socket = server_fd;
		sock_axs.sock_in = address; 
		return sock_axs;
}

//Socket escuchando
int socket_escucha(Socket_Access socket_axs){
	int addrlen = sizeof(socket_axs.sock_in);
	int new_socket;
	printf("\n+++++++ Waiting for new connection ++++++++\n\n");
	if ((new_socket = accept(socket_axs.fd_socket, (struct sockaddr *)&(socket_axs.sock_in), (socklen_t*)&addrlen))<0){
		if(new_socket == -1){
			printf("Son killed accept with signal\n");
			//Retry
			printf("\n+++++++ Waiting for new connection ++++++++\n\n");
			new_socket = accept(socket_axs.fd_socket, (struct sockaddr *)&(socket_axs.sock_in), (socklen_t*)&addrlen);
		}
	} 
	return new_socket;
}

//Lectura de lo enviado por un proceso client por el socket 
void socket_read(int new_socket, char *read_to_here){

	char buffer[30000] = {0};

        if(read(new_socket , buffer, 30000) == -1){ // -> lee lo enviado por un proceso cliente
		perror(strerror(errno));
	}
        printf("%s\n", buffer);
	
	if(strcmp(read_to_here, "empty") != 0){
		printf("read no es null\n");
		strcpy(read_to_here, buffer);
	}
}

//Respuesta de server a proceso client por el socket
void socket_response(int new_socket, char data_for_consola[MAX_SERVER_RESPONSE]){

	if(write(new_socket , data_for_consola, strlen(data_for_consola)) == -1){ // -> escribe la respuesta
		perror(strerror(errno));
	}
}

//Cerrar socket
void close_socket(int new_socket){
	printf("------------------Response message sent-------------------\n");
	close(new_socket);
}

//Cliente pidiendo conexion por socket
int socket_client(char request_sock[MAX_LENGTH_QUERY], char *data, int socket_port){

	int sock = 0; 
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Socket creation error \n");
		return -1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(socket_port);
    
	// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0){
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ // -> solicita conexion
		printf("\nConnection Failed \n");
		return -1;
	}

	if(send(sock , request_sock, strlen(request_sock), 0 ) == -1){ // -> se mando el request
		perror(strerror(errno));
	}
    printf("\nRequest sent via Internet Domain Socket!:\n%s\n", request_sock);

   	if(read(sock, buffer, 1024) == -1){ // -> se lee respuesta del servidor
		perror(strerror(errno));
	}
	
    printf("\nResponse:\n%s\n",buffer );

   	if (data != NULL) {
		strcpy(data, buffer);
	}

	return 0;
}


//Unix Domain Sockets

//Crea Socket
Socket_Access_Unix create_unix_socket(char socket_filename[SOCKET_FILENAME_MAX_SIZE]){ 

	    int server_fd;
	    struct sockaddr_un address;
	    Socket_Access_Unix sock_axs_unix;
	    
	    // Creating socket file descriptor
	    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0){
		perror("In socket");
		exit(EXIT_FAILURE);
	    }

	    memset(&address, 0, sizeof address);

	    address.sun_family = AF_UNIX;
	    strcpy(address.sun_path, socket_filename); //UNIX_DOMAIN_SOCKET_PATH_SERVER
	    unlink(socket_filename);

	    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0){
		perror("In bind");
		exit(EXIT_FAILURE);
	    }
	    if (listen(server_fd, 10) < 0){ //Maximo numero de conexiones en cola, despues se rechazan
		perror("In listen");
		exit(EXIT_FAILURE);
	    }
		sock_axs_unix.fd_unix_socket = server_fd;
		sock_axs_unix.sock_un = address; 
		return sock_axs_unix;
}


//Socket escuchando
int socket_unix_escucha(Socket_Access_Unix socket_axs_unix){
	int addrlen = sizeof(socket_axs_unix.sock_un);
	int new_socket;
	printf("\n+++++++ Waiting for new connection ++++++++\n\n");
	if ((new_socket = accept(socket_axs_unix.fd_unix_socket, (struct sockaddr *)&(socket_axs_unix.sock_un), (socklen_t*)&addrlen))<0){
		if(new_socket == -1){
			printf("Son killed accept with signal\n");
			//Retry
			printf("\n+++++++ Waiting for new connection ++++++++\n\n");
			new_socket = accept(socket_axs_unix.fd_unix_socket, (struct sockaddr *)&(socket_axs_unix.sock_un), 		  				(socklen_t*)&addrlen);
		}
	} 
	return new_socket;
}


//Lectura de lo enviado por un proceso client por el socket 
void socket_unix_read(int new_socket, char *read_to_here){

	char buffer[30000] = {0};

        if(read(new_socket , buffer, 30000) == -1){ // -> lee lo enviado por un proceso cliente
		perror(strerror(errno));
	}
        printf("Lectura de lo enviado por el socket:\n%s\n", buffer);
	strcpy(read_to_here, buffer);
	//if(strcmp(read_to_here, "empty") != 0){
	//	printf("read no es null\n");
	//	strcpy(read_to_here, buffer);
	//}
}

//Respuesta de server a proceso client por el socket
void socket_unix_response(int new_socket, char data_for_consola[MAX_SERVER_RESPONSE]){

	//char response_server[MAX_SERVER_RESPONSE] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 9\n\nSoy HTTP!";
	if(write(new_socket , data_for_consola, strlen(data_for_consola)) == -1){ // -> escribe la respuesta
		perror(strerror(errno));
	}
}

//Cerrar el socket
void close_unix_socket(int new_socket){
	printf("------------------Response message sent-------------------\n");
	close(new_socket);
}

//Cliente pidiendo conexion por socket
int socket_unix_client(char request_sock[MAX_LENGTH_QUERY], char *direcciones_Backends, char socket_filename[SOCKET_FILENAME_MAX_SIZE]){

	int sock = 0; 
	struct sockaddr_un serv_addr;
	char buffer[1024] = {0};
    	if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        	printf("\n Socket creation error \n");
       		return -1;
    	}
    
    	memset(&serv_addr, 0, sizeof serv_addr);

	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, socket_filename); //UNIX_DOMAIN_SOCKET_PATH_SERVER
	    

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ // -> solicita conexion
    	  printf("\nConnection Failed \n");
	  perror(strerror(errno));
          return -1;
        }
	
	if(send(sock , request_sock, strlen(request_sock), 0 ) == -1){ // -> se mando el request
		perror(strerror(errno));
	}
    	printf("\nRequest sent via Unix Domain!:%s\n",request_sock); 

   	if(read(sock, buffer, 1024) == -1){ // -> se lee respuesta del servidor
		perror(strerror(errno));
	}

    	printf("\nResponse:\n%s\n",buffer ); 
	strcpy(direcciones_Backends, buffer);
        return 0;
}


