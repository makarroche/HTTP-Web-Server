#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "semaphore.h"

// Definitions
union sumun{
	int val;
	struct semid_ds * buf;
	unsigned short int *array;
};


Semaphore semCreate(key_t key) {
	return semget(key, 1, 0666|IPC_CREAT);
}

int semInit(Semaphore s, int value){
	union sumun arg;
	arg.val = value;
	return semctl(s, 0, SETVAL, arg);
}

int semWait(Semaphore s){
	struct sembuf sb = {0, -1, 0};
	return semop(s, &sb, 1);
}

int semPost(Semaphore s){
	struct sembuf sb = {0, 1, 0};
	return semop(s, &sb, 1);
}

int semDestroy(Semaphore s){
	return semctl(s, 0, IPC_RMID);
}
