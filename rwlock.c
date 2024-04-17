#include "rwlock.h"
#include "common_threads.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

_Atomic unsigned long read_acquires = 0;
_Atomic unsigned long read_releases = 0;
_Atomic unsigned long write_acquires = 0;
_Atomic unsigned long write_releases = 0;
FILE* LOG_OUTPUT = NULL;

void rwlock_init(rwlock_t* lock) {
	lock->readers = 0;
	Sem_init(&lock->lock, 1);
	Sem_init(&lock->writelock, 1);
}

void rwlock_acquire_readlock(rwlock_t* lock) {
	Sem_wait(&lock->lock);
	lock->readers++;
	if (lock->readers == 1)
		Sem_wait(&lock->writelock);
	Sem_post(&lock->lock);

	read_acquires++;
	fputs("READ LOCK ACQUIRED\n", LOG_OUTPUT);
}

void rwlock_release_readlock(rwlock_t* lock) {
	Sem_wait(&lock->lock);
	lock->readers--;
	if (lock->readers == 0)
		Sem_post(&lock->writelock);
	Sem_post(&lock->lock);

	read_releases++;
	fputs("READ LOCK RELEASED\n", LOG_OUTPUT);
}

void rwlock_acquire_writelock(rwlock_t* lock) {
	Sem_wait(&lock->writelock);

	write_acquires++;
	fputs("WRITE LOCK ACQUIRED\n", LOG_OUTPUT);
}

void rwlock_release_writelock(rwlock_t* lock) {
	Sem_post(&lock->writelock);

	write_releases++;
	fputs("WRITE LOCK RELEASED\n", LOG_OUTPUT);
}
