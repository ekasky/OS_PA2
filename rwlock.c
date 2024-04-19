#include "rwlock.h"
#include "common_threads.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

_Atomic unsigned long read_acquires = 0;
_Atomic unsigned long read_releases = 0;
_Atomic unsigned long write_acquires = 0;
_Atomic unsigned long write_releases = 0;

void rwlock_init(rwlock_t* lock) {
	// Sets the number of reads currently held in the lock to 0
	lock->readers = 0;
	// Ensures multiple readers can access critcal section concurrently						
	Sem_init(&lock->lock, 1);
	// Ensures only one writer can access the crtical session at a time
	Sem_init(&lock->writelock, 1);
}

void rwlock_acquire_readlock(rwlock_t* lock) {
	// Wait until we have permission to enter the critcal section
	Sem_wait(&lock->lock);
	// Increment the number of reader counts by 1
	lock->readers++;
	if (lock->readers == 1)
	// If we are the first reader accquire the write lock to prevent other writers from entering the critcal section while readers are present (Prevent value from changing as a reader is obtaining it)
		Sem_wait(&lock->writelock);
	// Release the lock to allow other threads to access crtical section
	Sem_post(&lock->lock);

	read_acquires++;
	puts("READ LOCK ACQUIRED");
}

void rwlock_release_readlock(rwlock_t* lock) {
	// Wait until we have permission to enter the crtical section
	Sem_wait(&lock->lock);
	// Subtract one from the readers count
	lock->readers--;
	if (lock->readers == 0)
	// Release the write lock to allow a writer to accquire write permissions
		Sem_post(&lock->writelock);
	// Release the lokc to allow other threads to access critcal section
	Sem_post(&lock->lock);

	read_releases++;
	puts("READ LOCK RELEASED");
}

void rwlock_acquire_writelock(rwlock_t* lock) {
	//Wait until we have have aquired the writeLock
	Sem_wait(&lock->writelock);

	write_acquires++;
	puts("WRITE LOCK ACQUIRED");
}

void rwlock_release_writelock(rwlock_t* lock) {
	// Relase the write lock to allow other threads to accquire it for write
	Sem_post(&lock->writelock);

	write_releases++;
	puts("WRITE LOCK RELEASED");
}
