#ifndef __RW_LOCKS_H__
#define __RW_LOCKS_H__

#include <stdio.h>
#include <semaphore.h>
#include "common.h"
#include "common_threads.h"

typedef struct rwlock_t {

	sem_t writeLock;						// Used to control access to critcal section when writing
	sem_t lock;							// Used to control access to crtical section when reading
	int readers;							// Keeps track of the number of reads

} rwlock_t;

void rwlock_init(rwlock_t* lock);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t *lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);


/* ChatGPT generated functions (These functions are just for testing and will be removed) */
// Function to simulate reading operation

void *reader(void *arg) {
    rwlock_t *lock = (rwlock_t *)arg;
    rwlock_acquire_readlock(lock);
    printf("Reading...\n");
    rwlock_release_readlock(lock);
    return NULL;
}

// Function to simulate writing operation
void *writer(void *arg) {
    rwlock_t *lock = (rwlock_t *)arg;
    rwlock_acquire_writelock(lock);
    printf("Writing...\n");
    rwlock_release_writelock(lock);
    return NULL;
}

#endif
