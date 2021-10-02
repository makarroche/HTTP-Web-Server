#include "shared_memory.h"

SHM_Management shm_result;

//Pedir Shared Memory -> Segmento Nuevo
void request_shared_memory (SHM_Access *axs, char sh_filename[CAMPO_SHM_MAXSIZE], int project_key){

	key_t mykey;       
	int id;
	void *shmptr;

	// Crar el archivo temporal
	if (creat(sh_filename, TMPFILEMODE | O_CREAT | O_EXCL) < 0) { 
		SYSERROR(EXIT_FAILURE, "Error al crear archivo temporal.");	
	}
	
	//Get value of key
	if ((mykey = ftok(sh_filename, project_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al obtener el 'key'.");
	}
	printf("mykey = %d = %x\n", mykey, mykey);

	if ((id = shmget(mykey, SHMSIZE, IPC_CREAT | IPC_EXCL | 0666)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al solicitar memoria compartida.");
	}
	printf("id = %d\n", id);

	if ((shmptr = shmat(id, NULL, 0)) == ((void *)-1)) {
		SYSERROR(EXIT_FAILURE, "Error al mapear la memoria compartida.");
	}
	axs->shm_key = mykey;
	axs->shm_id = id;
	axs->shm_address = shmptr; 
}

//Inicializar el segmento
void initialize_shm_management(SHM_Access axs){
	SHM_Management shm_m, *shm_maux;
	int j = 0;

	shm_m.solicitudes_efectivas = 0;
	shm_m.solicitudes_rechazadas = 0;
	shm_m.respuestas_efectivas = 0;
	shm_m.respuestas_con_error = 0;

	
	shm_maux = (SHM_Management *) (axs.shm_address);
	memmove(shm_maux, &shm_m, sizeof(SHM_Management));

	while(j<3){

		//semWait(axs.shm_id);

		// REGION CRITICA!!!!
		// *****************************************************************************
		printf("solicitudes efectivas=%d, solicitudes rechazadas=%d, respuestas efectivas=%d, respuestas con error=%d\n",
			shm_maux->solicitudes_efectivas, shm_maux->solicitudes_rechazadas, shm_maux->respuestas_efectivas, 				shm_maux->respuestas_con_error);	
		// *****************************************************************************

		//semPost(axs.shm_id);
		j++;
		sleep(1);
	}

}


//Preguntar Shared Memory -> Segmento Existente
void ask_shared_memory(SHM_Access *axs, char sh_filename[CAMPO_SHM_MAXSIZE], int project_key){

	key_t mykey;       
	int id;
	void *shmptr;

	// Crar el archivo temporal
	if (creat(sh_filename, TMPFILEMODE | O_CREAT) < 0) {
		SYSERROR(EXIT_FAILURE, "Error al crear archivo temporal.");	
	}
	
	//Get value of key
	if ((mykey = ftok(sh_filename, project_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al obtener el 'key'.");
	}
	printf("mykey = %d = %x\n", mykey, mykey);

	if ((id = shmget(mykey, SHMSIZE, IPC_CREAT | 0666)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al solicitar memoria compartida.");
	}
	printf("id = %d\n", id);

	if ((shmptr = shmat(id, NULL, 0)) == ((void *)-1)) {
		SYSERROR(EXIT_FAILURE, "Error al mapear la memoria compartida.");
	}
	axs->shm_key = mykey;
	axs->shm_id = id;
	axs->shm_address = shmptr;  
}

//Escribir en Shared Memory Management
void shm_management_and_semaphore_write(SHM_Access axs, char campo[CAMPO_SHM_MAXSIZE]){

	Semaphore semaf;
	SHM_Management *shm_maux;

	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}
	// Inicializar semaforo
	semInit(semaf, 1);

	// ***************************************************************************
	// AREA DE TRABAJO
	// ***************************************************************************
	
	shm_maux = (SHM_Management *) (axs.shm_address);
	int *point_campo;

	if(strcmp(campo, "solicitud_efectiva") == 0){
		point_campo = &(shm_maux->solicitudes_efectivas);
	}
	if(strcmp(campo, "solicitud_rechazada") == 0){
		point_campo = &(shm_maux->solicitudes_rechazadas);
	}
	if(strcmp(campo, "respuesta_efectiva") == 0){
		point_campo = &(shm_maux->respuestas_efectivas);
	}
	if(strcmp(campo, "respuesta_error") == 0){
		point_campo = &(shm_maux->respuestas_con_error);
	}

		// Actualizar Shared Memory Management
		semWait(axs.shm_id);

		// REGION CRITICA!!!!
		// *****************************************************************************
		critical_region(point_campo);	
		// *****************************************************************************

		semPost(axs.shm_id);
		printf("Shared Memory Management Actualizada Correctamente\n");
}

//Leer desde Shared Memory Management
SHM_Management shm_management_and_semaphore_read(SHM_Access axs){
	
	Semaphore semaf;
	SHM_Management *shm_read, shm_result;


	if (( semaf = semCreate(axs.shm_key)) == -1 ) {
			SYSERROR(EXIT_FAILURE, "Error al crear el semaforo.");
		}	

		// ***************************************************************************
		// AREA DE TRABAJO
		// ***************************************************************************

		shm_read = (SHM_Management*) (axs.shm_address);
	
	
			// Leer de Shared Memory Management
			semWait(semaf);
			// ***************************************************************************
			// Region critica
			shm_result.solicitudes_efectivas = shm_read->solicitudes_efectivas;
			shm_result.solicitudes_rechazadas = shm_read->solicitudes_rechazadas;
			shm_result.respuestas_efectivas = shm_read->respuestas_efectivas;
			shm_result.respuestas_con_error = shm_read->respuestas_con_error;
			// ***************************************************************************
			// NOTA: no es conveniente utilizar una funcion de I/O en la region critica
				
			// ***************************************************************************
			semPost(semaf);
			// ***************************************************************************

	return shm_result;
}

//Region critica Shared Memory Management
void critical_region(int *point_campo){
	int aux = *point_campo;
	aux++; // Aumenta en 1 el campo adecuado en Shared Memory Management 

	*point_campo = aux; 
}

void agregar_backend(SHM_Backend_Mapping* bemap, int backend_id) {
	bemap->backend_id[bemap->backend_count] = backend_id;
	bemap->backend_carga[bemap->backend_count] = 0;
	bemap->backend_count++;
}

void borrar_backend(SHM_Backend_Mapping* bemap, int backend_id) {
	int i;
	for ( i = 0; i < bemap->backend_count; ++i) {
		if (bemap->backend_id[i] == backend_id) {
			bemap->backend_id[i] = bemap->backend_id[bemap->backend_count] ;
			bemap->backend_carga[i] = bemap->backend_carga[bemap->backend_count] ;
			break;
		}
	}
	bemap->backend_count--;
}

int listar_menor_carga_backend(SHM_Backend_Mapping* bemap) {
	int idx = 0;
	int i;
	for ( i = 1; i < bemap->backend_count; ++i) {
		if (bemap->backend_carga[i] < bemap->backend_carga[idx]) {
			idx = i;
		}
	}

	// sumo un request ( el actual )
	bemap->backend_carga[idx]++;
	return bemap->backend_id[idx];
}

void mostrar_carga_backends(SHM_Backend_Mapping* bemap, char *be_cargas) {
	int i = 0;
	char *aux = "/0";
	for ( i = 1; i < bemap->backend_count; ++i) {
		sprintf(aux, "Carga Backend %d: %d", i, bemap->backend_carga[i]);
		strcat(be_cargas, aux);
	}
	printf("Backends cargas: %s\n", be_cargas);
}

void listar_puertos_backend(SHM_Backend_Mapping* bemap, Cadetes* backs) {
	int i;
	for ( i = 0; i < bemap->backend_count; ++i) {
		backs->ids[i] = bemap->backend_id[i];
	}
	backs->length = bemap->backend_count;

}

void agregar_path(SHM_Backend_Mapping* bemap, char path[MAX_PATH]) {

	memset(bemap->backend_paths[bemap->path_count], '\0', 
		sizeof(bemap->backend_paths[bemap->path_count]));

	strcpy(bemap->backend_paths[bemap->path_count], path);
	bemap->path_count++;
}

void borrar_path(SHM_Backend_Mapping* bemap, char path[MAX_PATH]) {
	int i;
	for ( i = 0; i < bemap->backend_count; ++i) {
		if (strcmp(bemap->backend_paths[i], path) == 0) {
			
			memset(bemap->backend_paths[i], '\0', 
				sizeof(bemap->backend_paths[i]));

			strcpy(bemap->backend_paths[i], bemap->backend_paths[bemap->path_count]);
			break;
		}
	}
	bemap->path_count--;
}


// Agregar Backend en Shared Memory Backend Mapping
void shm_be_mapping_and_semaphore_write_add_be(SHM_Access axs, int be_id){

	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}

	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(semaf);

	// REGION CRITICA!!!!
	// *****************************************************************************
	agregar_backend(shm_maux, be_id);
	// *****************************************************************************

	semPost(semaf);
	printf("Shared Memory Mapping Actualizada Correctamente\n");
}

// Borrar Backend de Shared Memory Backend Mapping
void shm_be_mapping_and_semaphore_write_del_be(SHM_Access axs, int be_id){

	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}
	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(axs.shm_id);

	// REGION CRITICA!!!!
	// *****************************************************************************
	borrar_backend(shm_maux, be_id);
	// *****************************************************************************

	semPost(axs.shm_id);
	printf("Shared Memory Management Actualizada Correctamente\n");
}

// Puerto con menor carga en Shared Memory Backend Mapping
int shm_be_mapping_and_semaphore_write_menor_be(SHM_Access axs){

	int menor_carga = 0;
	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}

	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(axs.shm_id);

	// REGION CRITICA!!!!
	// *****************************************************************************
	menor_carga = listar_menor_carga_backend(shm_maux);
	// *****************************************************************************

	semPost(axs.shm_id);
	printf("Balance de carga realizado Correctamente, se devolvio puerto de Backend con menor Carga\n");

	return menor_carga;
}

// Leer puertos Backends de Shared Memory Backend Mapping
void shm_be_mapping_and_semaphore_read_ports(SHM_Access axs, Cadetes* backs) {

	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}

	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(axs.shm_id);

	// REGION CRITICA!!!!
	// *****************************************************************************
	listar_puertos_backend(shm_maux, backs);
	// *****************************************************************************

	semPost(axs.shm_id);
	printf("Shared Memory Management Actualizada Correctamente\n");

}

// Agregar Path en Shared Memory Backend Mapping
void shm_be_mapping_and_semaphore_write_add_path(SHM_Access axs, char path[MAX_PATH]){

	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}
	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(axs.shm_id);

	// REGION CRITICA!!!!
	// *****************************************************************************
	agregar_path(shm_maux, path);
	// *****************************************************************************

	semPost(axs.shm_id);
	printf("Shared Memory Management Actualizada Correctamente\n");
}

// Borrar Path de Shared Memory Backend Mapping
void shm_be_mapping_and_semaphore_write_del_path(SHM_Access axs, char path[MAX_PATH]){

	Semaphore semaf;
	SHM_Backend_Mapping *shm_maux;
	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}
	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);

	semWait(axs.shm_id);

	// REGION CRITICA!!!!
	// *****************************************************************************
	borrar_path(shm_maux, path);
	// *****************************************************************************

	semPost(axs.shm_id);
	printf("Shared Memory Management Actualizada Correctamente\n");
}


//Inicializar el segmento
void initialize_shm_backend_mapping(SHM_Access axs){
	int i = 0;
	int j = 0;
	SHM_Backend_Mapping shm_m, *shm_maux;
	Semaphore semaf;

	// Crear semaforo
	if ((semaf = semCreate(axs.shm_key)) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al crear semaforo.");
	}
	// Inicializar semaforo
	semInit(semaf, 1);

	// Inicializa la memoria compartida.
	shm_m.backend_count = 0;
	shm_m.path_count = 0;

	for (i = 0; i < MAX_PATH; ++i)
	{
		for (j = 0; j < URL_MAXSIZE; ++j)
		{
			
			shm_m.backend_paths[i][j] = '\0';
		}
	}
	
	shm_maux = (SHM_Backend_Mapping *) (axs.shm_address);
	memmove(shm_maux, &shm_m, sizeof(SHM_Backend_Mapping));

}


//Detach de Shared Memory
void detach_shared_memory(SHM_Access axs){
	shmdt(axs.shm_address);
}


//Borrar Shared Memory
void finish_shared_memory(SHM_Access axs, char sh_filename[CAMPO_SHM_MAXSIZE] ){
	// Marcado para borrar
	shmctl(axs.shm_id, IPC_RMID, NULL);

	// Borrar el archivo TMPFILENAME con 'unlink'
	if(unlink(sh_filename) == -1) {
		SYSERROR(EXIT_FAILURE, "Error al borrar archivo temporal.");
	}
}



