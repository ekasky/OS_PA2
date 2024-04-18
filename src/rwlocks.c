/* 

I used the following link as a guide and resource when writing this code:
	https://github.com/remzi-arpacidusseau/ostep-code/blob/master/threads-sema/rwlock.c
	
I also used ChatGPT 3.0-Turbo to help break down each function and understand what they do:
	https://chat.openai.com/share/2daf80be-c504-4ad6-8c54-791f1e302a95
`*/

#include "../includes/rwlocks.h"

/* Initializing the lock */
rwlock_t* rwlock_init() {

	rwlock_t* lock = (rwlock_t*)calloc(1, sizeof(rwlock_t));

	if(!lock) {

		fprintf(stderr, "[ERROR]: Could not allocate space for lock\n");
		exit(1);

	}

	/* Sets the number of reads currently held in the lock to 0 */
	lock->readers = 0;						
	/* Ensures multiple readers can access critcal section concurrently */
	Sem_init(&lock->lock, 1);
	/* Ensures only one writer can access the crtical session at a time */
	Sem_init(&lock->writeLock, 1);

	return lock;

}

/* Function to assign the read lock */
void rwlock_acquire_read_lock(rwlock_t* lock, FILE* fp, int* count) {

	/* Wait until we have permission to enter the critcal section */
	Sem_wait(&lock->lock);
	/* Increment the number of reader counts by 1 */
	lock->readers++;
	
	if(lock->readers == 1) {
	
		/* If we are the first reader accquire the write lock to prevent other writers from entering the critcal section
		 while readers are present (Prevent value from changing as a reader is obtaining it )*/
		Sem_wait(&lock->writeLock);
	
	}

	fprintf(fp, "READ LOCK ACQUIRED\n");
	
	/* Release the lock to allow other threads to access crtical section */
	Sem_post(&lock->lock);					

	*count = *count + 1;

}

/* Function to release the reading lock */
void rwlock_release_read_lock(rwlock_t *lock, FILE* fp, int* count) {

	/* Wait until we have permission to enter the crtical section */
	Sem_wait(&lock->lock);
	/* Subtract one from the readers count */
	lock->readers--;
	
	if(&lock->lock) {
	
		/* Release the write lock to allow a writer to accquire write permissions  */
		Sem_post(&lock->writeLock);
	
	}

	fprintf(fp, "READ LOCK RELEASED\n");
	
	/* Release the lock to allow other threads to access critcal section */
	Sem_post(&lock->lock);

	*count = *count + 1;

}

/* Function to assign the writing lock */
void rwlock_acquire_write_lock(rwlock_t* lock, FILE* fp, int* count) {

	/* Wait until we have have aquired the writeLock */
	Sem_wait(&lock->writeLock);	
	fprintf(fp, "WRITE LOCK ACQUIRED\n");

	*count = *count + 1;

}

/* Function to release the writing lock */
void rwlock_release_write_lock(rwlock_t* lock, FILE* fp, int* count) {

	/* Relase the write lock to allow other threads to accquire it for write */
	Sem_post(&lock->writeLock);
	fprintf(fp, "WRITE LOCK RELEASED\n");

	*count = *count + 1;

}

