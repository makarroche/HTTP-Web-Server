#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

typedef int Semaphore;

// semCreate returns -1 if error
Semaphore semCreate(key_t key);

int semInit(Semaphore s, int value);

int semWait(Semaphore s);

int semPost(Semaphore s);

// semDestroy returns -1 if error
int semDestroy(Semaphore s);

#endif
