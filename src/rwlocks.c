/* 

I used the following link as a guide and resource when writing this code:
	https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
	
I also used ChatGPT 3.0-Turbo to help break down each function and understand what they do:
	https://chat.openai.com/share/2daf80be-c504-4ad6-8c54-791f1e302a95
`*/

#include "../includes/rwlocks.h"

void rwlock_init(rwlock_t* lock) {

	lock->readers = 0;					// Sets the number of reads currently held in the lock to 0
	Sem_init(&lock->lock, 1);				// Ensures multiple readers can access critcal section concurrently
	Sem_init(&lock->writeLock, 1);				// Ensures only one writer can access the crtical session at a time

}

void rwlock_acquire_read_lock(rwlock_t* lock) {

	Sem_wait(&lock->lock);					// Wait until we have permission to enter the critcal section
	lock->readers++;					// Increment the number of reader counts by 1
	
	if(lock->readers == 1) {
	
		Sem_wait(&lock->writeLock);			// If we are the first reader accquire the write lock to prevent other writers from entering the critcal section while readers are present (Prevent value from changing as a reader is obtaining it)
	
	}
	
	Sem_post(&lock->lock);					// Release the lock to allow other threads to access crtical section

}

void rwlock_release_read_lock(rwlock_t *lock) {

	Sem_wait(&lock->lock);					// Wait until we have permission to enter the crtical section
	lock->readers--;					// Subtract one from the readers count
	
	if(&lock->lock) {
	
		Sem_post(&lock->writeLock);			// Release the write lock to allow a writer to accquire write permissions 
	
	}
	
	Sem_post(&lock->lock);					// Release the lokc to allow other threads to access critcal section

}

void rwlock_acquire_write_lock(rwlock_t* lock) {

	Sem_wait(&lock->writeLock);				//Wait until we have have aquired the writeLock

}

void rwlock_release_write_lock(rwlock_t* lock) {

	Sem_post(&lock->writeLock);				// Relase the write lock to allow other threads to accquire it for write

}
