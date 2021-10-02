#ifndef _Estructuras_ 
#define _Estructuras_

#define METHOD_MAXSIZE 7
#define URL_MAXSIZE 20
#define LIST_BACKEND_ID_MAXSIZE 103
#define CONTENT_LENGTH_MAXSIZE 100
#define CONTENT_TYPE_MAXSIZE 20
#define HOST 17 
#define MAX_AMOUNT_DATA 1024
#define REQUEST_STATUS_MAXSIZE 9
#define MAX_CHILDREN 16
#define MAX_PATH 216

// Socket entre Console magager y HBE   
#define BE_CONTROL_PORT 5999
#define CMD_AGREGAR "A"
#define CMD_QUITAR "Q"
#define CMD_SALIR "S"

// Utilizados para manejo de comunicacion entre el frontend y el cadete.
#define VERB_GET "GET"
#define VERB_PUT "PUT"
#define VERB_DELETE "DELETE"
#define VERB_SYNC_PUT "SYNCP"
#define VERB_SYNC_DELETE "SYNCD"
#define VERB_LIST "LIST" // solo para el personal manager. lista el directorio donde esta corriendo.


// Constantes para la Shared Memory Mapping
#define TMPFILENAMEMAP "shm_mapping"

//Estructura Requests
typedef struct {
	char request_status[REQUEST_STATUS_MAXSIZE];
	char method[METHOD_MAXSIZE];
	char url[URL_MAXSIZE];
	char host[HOST];
	char content_length[CONTENT_LENGTH_MAXSIZE];
	char content_type[CONTENT_TYPE_MAXSIZE];
	char message_body[MAX_AMOUNT_DATA];	
} Request;


//Estructura Datos Shared Memory Management
typedef struct {
	int solicitudes_efectivas;
	int solicitudes_rechazadas;
	int respuestas_efectivas;
	int respuestas_con_error;
} SHM_Management;


//Estructura Datos Shared Memory Backend Mapping
typedef struct {
	int backend_id[MAX_CHILDREN];
	int backend_carga[MAX_CHILDREN];
	int backend_count;
	char backend_paths[MAX_PATH][URL_MAXSIZE];
	int path_count;
} SHM_Backend_Mapping;


//Estructura para administrar accesos a Shared Memory
typedef struct {
	key_t shm_key;
	int shm_id;
	void *shm_address;
} SHM_Access;


//Estructura Recurso
typedef struct {
	char content_length_resource[CONTENT_LENGTH_MAXSIZE];
	char content_type_resource[CONTENT_TYPE_MAXSIZE];
	char message_body_resource[MAX_AMOUNT_DATA];
} Resource;


//Estructura de construccion de socket InternetDS
typedef struct {
	int fd_socket;
	struct sockaddr_in sock_in;
	
} Socket_Access;


//Estructura lista PID hijos
typedef struct {
	pid_t pids[MAX_CHILDREN];
	int ids[MAX_CHILDREN];
	int length;
} PidsList;


//Estructura Cadetes
typedef struct {
    int ids[MAX_CHILDREN];
    int length;
} Cadetes;


//Estructura Construccion de UnixDS
typedef struct {
	int fd_unix_socket;
	struct sockaddr_un sock_un;
	
} Socket_Access_Unix;

#endif
