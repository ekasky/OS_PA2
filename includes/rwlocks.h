#ifndef __RW_LOCKS_H__
#define __RW_LOCKS_H__

#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "common_threads.h"

typedef struct rwlock_t {

	sem_t writeLock;						// Used to control access to critcal section when writing
	sem_t lock;							// Used to control access to crtical section when reading
	int readers;							// Keeps track of the number of reads

} rwlock_t;

rwlock_t* rwlock_init();
void rwlock_acquire_read_lock(rwlock_t* lock);
void rwlock_release_read_lock(rwlock_t *lock);
void rwlock_acquire_write_lock(rwlock_t* lock);
void rwlock_release_write_lock(rwlock_t* lock);

#endif
