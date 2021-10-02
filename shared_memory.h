#ifndef _SHARED_MEMORY_
#define _SHARED_MEMORY_

#include "includes.h"

#define CAMPO_SHM_MAXSIZE 30
#define TMPFILEMODE O_RDWR
#define SHMSIZE 1024*1024

//Pedir Shared Memory -> Segmento Nuevo
void request_shared_memory (SHM_Access *axs, char sh_filename[CAMPO_SHM_MAXSIZE], int project_key);

//Inicializar el segmento Shared Memory Management
void initialize_shm_management(SHM_Access axs);

//Preguntar Shared Memory -> Segmento Existente
void ask_shared_memory(SHM_Access *axs, char sh_filename[CAMPO_SHM_MAXSIZE], int project_key);

//Escribir en Shared Memory Management
void shm_management_and_semaphore_write(SHM_Access axs, char campo[]);

//Leer de Shared Memory Management
SHM_Management shm_management_and_semaphore_read(SHM_Access axs);

//Region critica Shared Memory Management
void critical_region(int *point_campo);

void shm_be_mapping_and_semaphore_write_add_be(SHM_Access axs, int be_id);

void shm_be_mapping_and_semaphore_write_del_be(SHM_Access axs, int be_id);

int shm_be_mapping_and_semaphore_write_menor_be(SHM_Access axs);

void shm_be_mapping_and_semaphore_read_ports(SHM_Access axs, Cadetes* backs);

void shm_be_mapping_and_semaphore_write_add_path(SHM_Access axs, char path[MAX_PATH]);

void shm_be_mapping_and_semaphore_write_del_path(SHM_Access axs, char path[MAX_PATH]);

void mostrar_carga_backends(SHM_Backend_Mapping* bemap, char *be_cargas);

//Inicializar el segmento Shared Memory Management
void initialize_shm_backend_mapping(SHM_Access axs);

//Inicializar el segmento Shared Memory Management
void initialize_shm_backend_mapping(SHM_Access axs);

//Detach de Shared Memory
void detach_shared_memory(SHM_Access axs);

//Borrar Shared Memory
void finish_shared_memory(SHM_Access axs, char sh_filename[CAMPO_SHM_MAXSIZE]);


#endif
