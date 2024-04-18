#ifndef __RWLOCKS_H__
#define __RWLOCKS_H__

#include <pthread.h>

typedef struct rwlock_t
{
    pthread_mutex_t lock;
    pthread_cond_t read;
    pthread_cond_t write;
    int readers;
    int writers;
    int waiting_writers;
} rwlock_t;

void rwlock_init(rwlock_t *rw);
void rwlock_acquire_readlock(rwlock_t *rw);
void rwlock_release_readlock(rwlock_t *rw);
void rwlock_acquire_writelock(rwlock_t *rw);
void rwlock_release_writelock(rwlock_t *rw);

#endif // __RWLOCKS_H__
