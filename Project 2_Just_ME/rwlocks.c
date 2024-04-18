#include "rwlocks.h"

void rwlock_init(rwlock_t *rw)
{
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->read, NULL);
    pthread_cond_init(&rw->write, NULL);
    rw->readers = 0;
    rw->writers = 0;
    rw->waiting_writers = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw)
{
    pthread_mutex_lock(&rw->lock);
    while (rw->writers > 0 || rw->waiting_writers > 0)
    {
        pthread_cond_wait(&rw->read, &rw->lock);
    }
    rw->readers++;
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw)
{
    pthread_mutex_lock(&rw->lock);
    rw->readers--;
    if (rw->readers == 0 && rw->waiting_writers > 0)
    {
        pthread_cond_signal(&rw->write);
    }
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw)
{
    pthread_mutex_lock(&rw->lock);
    rw->waiting_writers++;
    while (rw->writers > 0 || rw->readers > 0)
    {
        pthread_cond_wait(&rw->write, &rw->lock);
    }
    rw->waiting_writers--;
    rw->writers++;
    pthread_mutex_unlock(&rw->lock);
}

void rwlock_release_writelock(rwlock_t *rw)
{
    pthread_mutex_lock(&rw->lock);
    rw->writers--;
    if (rw->waiting_writers > 0)
    {
        pthread_cond_signal(&rw->write);
    }
    else
    {
        pthread_cond_broadcast(&rw->read);
    }
    pthread_mutex_unlock(&rw->lock);
}
