#include "includes.h"
#include "sock.h"
#include "socket.h" 

//Recibe solicitud de Cadete por el Socket

//Definicion de las funciones
void set_signal(void *handler, int signal_type);

void handle_list(int socket);
void handle_get(int socket);
void handle_put(int socket);
void handle_delete(int socket);

int read_file_content(char file_path[256], void** dato);
int write_file_content(char file_path[256], char* content);

int mymkdir(char *pathdir, char *file);

int continuar = 1;

//Funcion para capturar SIGCHLD
void capture_sig(int signal, siginfo_t *info, void *context) {

    int status;

    if(wait(&status) == -1){;
        SYSERROR(EXIT_FAILURE,"Error en el wait");
    }
    else{
        continuar = 0;
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char **argv) {

    if (argc != 2) {
        printf("Cantidad de parametros inadecuada. Uso: ./personal_manager <port>\n");
        exit(EXIT_FAILURE);
        SYSERROR(EXIT_FAILURE, "Cantidad de parametros inadecuada. Uso: ./personal_manager <port>\n");
    }

    // para cortar el procesamiento
    set_signal(capture_sig, SIGINT);
    set_signal(capture_sig, SIGTERM);

    int listening_port = atoi(argv[1]);

    // bind del socket
    int sock_fd;
    sock_fd = sock_listen(listening_port);
    int socket;



    while (continuar > 0) {

        void *msg_heap;
        char *msg;
        int msg_size;

        // aceptar conexiones desde afuera.
        socket = sock_open(sock_fd);
        printf("Recibi la conexion del cadete.\n");

        // leer el comando que llega desde el cadete.
        socket_recv(socket, &msg_heap, &msg_size);
        msg = (char *) msg_heap;
        msg[msg_size] = '\0';

        printf("Se recibio el comando [%s] \n", msg);

        if (strcmp(msg, VERB_LIST) == 0) {
            // muestra el esquema del sistema de archivos donde se esta corriendo.
            handle_list(socket);

        } else if (strcmp(msg, VERB_GET) == 0) {
            // recibe primero el verbo HTTP (GET, PUT, ...)
            // caso del GET, recibe el path en una segunda instancia.
            handle_get(socket);

        } else if (strcmp(msg, VERB_PUT) == 0) {
            // para el PUT.
            // recibe el path y luego el contenido
            handle_put(socket);

        } else if (strcmp(msg, VERB_DELETE) == 0) {
            // caso del DELETE, recibe el path en una segunda instancia.
            handle_delete(socket);

        } else {
            socket_send(socket, "-1", 2);
        }

        free(msg_heap);
        socket_close(socket);
    }

    exit(EXIT_SUCCESS);
}


int read_file_content(char file_path[256], void** dato) {
    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        return -1;
	printf("Problema en read_file_content\n");
    }
	
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);
    printf("FILE READ CONTENT:%s of FILE PATH:%s \n", string, file_path); 
    string[fsize] = '\0';

    memcpy(dato, &string, sizeof(void *));
    

    return 0;
}

int write_file_content(char file_path[256], char* content) {
    FILE *f = fopen(file_path, "w+");
    if (f == NULL) {
        return -1;
    }
    fprintf(f, "%s", content);
    fclose(f);
    return 0;
}


void handle_list(int socket) {

}

void handle_get(int socket) {
    void *msg_heap;
    char *msg;
    int msg_size;

    // leer el path.
    socket_recv(socket, &msg_heap, &msg_size);

    // este buffer es para el contenido del archivo.
    void* buff_heap;
    char file_path[256];

    // agrega el punto para que no vaya a buscar al directorio root /, de linux.
    sprintf(file_path, ".%s", (char*)msg_heap);
    file_path[msg_size+1] = '\0';
    printf("GET %s\n", file_path);
    free(msg_heap);
    int status = read_file_content(file_path, &buff_heap);
    if (status < 0) {
        printf("ERROR\n");
        socket_send(socket, "-1", 2);
    } else {
        msg = (char*)buff_heap;
	printf("El contenido del archivo para Handler_Frontend_Child: %s\n", msg);
        socket_send(socket, msg, strlen(msg));
        free(buff_heap);
	
    }
}

void handle_put(int socket) {

    void *msg_heap;
    char* msg;
    int msg_size;
    char file_path[256];
    char file_path_aux[256];
    char file_name[50];

    // leer el path.
    socket_recv(socket, &msg_heap, &msg_size);

    // agrega el punto para que no vaya a buscar al directorio root /, de linux.
    msg = (char*)msg_heap;
    msg[msg_size] = '\0';
    sprintf(file_path_aux, ".%s", msg);
    file_path_aux[msg_size + 1] = '\0';
    free(msg_heap);
    
    strcpy(file_path, file_path_aux);
    //Lo recibe sin punto
    if(mymkdir(file_path, file_name) == -1){
    	printf("Error en mkdir creando el directorio"); 
    }
    
    
    printf("FILE NAME:%s\n", file_name);
    printf("FILE PATH:%s\n", file_path_aux);
	
    // este buffer es para el contenido del archivo.
    socket_recv(socket, &msg_heap, &msg_size);

    msg = (char*)msg_heap;
    msg[msg_size] = '\0'; 
    printf("ESTOOOO: %s\n", (char*)msg_heap);
	
    int status = write_file_content(file_path_aux, (char*)msg_heap);
    free(msg_heap);

    if (status < 0) {
        printf("ERROR\n");
        socket_send(socket, "PUT -1", 6);
    } else {
        socket_send(socket, "PUT OK", 6);

    }
}
void handle_delete(int socket) {

    void *msg_heap;
    char* msg;
    int msg_size;
    char file_path[256];

    // leer el path.
    socket_recv(socket, &msg_heap, &msg_size);

    // agrega el punto para que no vaya a buscar al directorio root /, de linux.
    msg = (char*)msg_heap;
    msg[msg_size] = '\0';
    sprintf(file_path, ".%s", msg);
    file_path[msg_size+1] = '\0';
    free(msg_heap);

    int status = unlink(file_path);

    if (status < 0) {
        printf("ERROR\n");
        socket_send(socket, "DELETE -1", 9);
    } else {
        socket_send(socket, "DELETE OK", 9);

    }
}


void set_signal (void *handler, int signal_type){

    // Estructura de sigaction
    struct sigaction action_data;

    // Set signal handler
    action_data.sa_sigaction = handler;

    // Use signal flags
    action_data.sa_flags = SA_SIGINFO;

    // Install signal action
    if (sigaction(signal_type, &action_data, NULL) == -1) {
        perror(strerror(errno));
        exit(EXIT_FAILURE);
    }
}


int mymkdir(char *pathdir, char *file_name) {
	printf("pathdir:%s\n", pathdir);
	// retorna -1 si hay error
	// retorna cantidad de directorios creados
	int i = 0;
	struct stat st;
	char *dir;
	pathdir++;
	char nombre_archivo[50];
	char path_dir_aux[50];
	// reservo un  buffer auxiliar para contener copia de pathdir
	char *aux = (char *) malloc(sizeof(char) * (strlen(pathdir) + 1 + 1));
	// inicializo el buffer como una cadena vacia.
	*aux = '\0'; 
	strcpy(path_dir_aux, pathdir);
	while ((dir = strtok(pathdir, "/")) != NULL) {
		printf("dir:%s\n", dir);
		
		strcat(aux, dir);
		
		if (stat(aux, &st) == 0) {
			// el "archivo" existe, ver que tipo es
			if (S_ISDIR(st.st_mode)) {
				printf("EXISTE:%d\n", S_ISDIR(st.st_mode));
				printf("Directorio %s ya existe\n", aux);
			} else {
				printf("Ya existe 'algo' con nombre %s\n", aux);
				//Le paso al archivo por arriba
			}
		} 
		else{ 
			printf("AUXILIAR:%s\n", aux);
			if (strstr(aux, ".txt") == NULL && strstr(aux, ".html") == NULL){ //No existe y es directorio
			
			// no hay "archivo", crear el directorio
			printf("Crear Directorio: %s\n", dir);
			sprintf(nombre_archivo, "/%s", dir);
			printf("Potencial nombre de archivo:%s\n", nombre_archivo);
				if(strcmp(path_dir_aux, nombre_archivo ) != 0){ //Si no es un archivo
					if (mkdir(aux, 0755) == -1) {
						printf("ERROR, no se pudo crear directorio %s\n", aux);
						return -1;
					}
					printf("Se crea directorio %s\n", aux);
					i++; // contar cuantos directorios se crean
				}
			}
		}
		strcat(aux, "/");
		pathdir = NULL;
		strcpy(file_name, dir);
		sprintf(file_name, "./%s\n", dir);
	}
	free(aux);
	return i;
}

