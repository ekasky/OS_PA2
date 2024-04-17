#pragma once
#include <semaphore.h>
#include <stdio.h>

typedef struct _rwlock_t {
	sem_t writelock;
	sem_t lock;
	int readers;
} rwlock_t;

extern _Atomic unsigned long read_acquires;
extern _Atomic unsigned long read_releases;
extern _Atomic unsigned long write_acquires;
extern _Atomic unsigned long write_releases;
// Code structure forced my hand, I'm sorry
extern FILE* LOG_OUTPUT;

void rwlock_init(rwlock_t* lock);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
