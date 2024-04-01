#ifndef __RW_LOCKS_H__
#define __RW_LOCKS_H__

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

#endif
