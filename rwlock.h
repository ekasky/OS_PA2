#include <semaphore.h>

typedef struct _rwlock_t {
	sem_t writelock;
	sem_t lock;
	int readers;
} rwlock_t;

_Atomic unsigned long read_acquires;
_Atomic unsigned long read_releases;
_Atomic unsigned long write_acquires;
_Atomic unsigned long write_releases;

void rwlock_init(rwlock_t* lock);
void rwlock_acquire_readlock(rwlock_t* lock);
void rwlock_release_readlock(rwlock_t* lock);
void rwlock_acquire_writelock(rwlock_t* lock);
void rwlock_release_writelock(rwlock_t* lock);
