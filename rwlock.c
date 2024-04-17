#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "common_threads.h"

_Atomic unsigned long read_acquires = 0;
_Atomic unsigned long read_releases = 0;
_Atomic unsigned long write_acquires = 0;
_Atomic unsigned long write_releases = 0;

typedef struct _rwlock_t {
	sem_t writelock;
	sem_t lock;
	int readers;
} rwlock_t;

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
	puts("READ LOCK ACQUIRED");
}

void rwlock_release_readlock(rwlock_t* lock) {
	Sem_wait(&lock->lock);
	lock->readers--;
	if (lock->readers == 0)
		Sem_post(&lock->writelock);
	Sem_post(&lock->lock);

	read_releases++;
	puts("READ LOCK RELEASED");
}

void rwlock_acquire_writelock(rwlock_t* lock) {
	Sem_wait(&lock->writelock);

	write_acquires++;
	puts("WRITE LOCK ACQUIRED");
}

void rwlock_release_writelock(rwlock_t* lock) {
	Sem_post(&lock->writelock);

	write_releases++;
	puts("WRITE LOCK RELEASED");
}
