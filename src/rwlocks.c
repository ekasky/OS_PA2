/* 

I used the following link as a guide and resource when writing this code:
	https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
 */

#include "../includes/rwlocks.h"

void rwlock_init(rwlock_t* lock) {

	lock->reader = 0;					// Sets the number of reads currently held in the lock to 0
	Sem_init(&lock->lock, 1);				// Ensures multiple readers can access critcal section concurrently
	Sem_init(&lock->writeLock, 1);				// Ensures only one writer can access the crtical session at a time

}
