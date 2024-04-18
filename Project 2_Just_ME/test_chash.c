#include "hashdb.h"
#include "common.h"
#include <stdio.h>
#include <pthread.h>
#include <assert.h>

void *thread_insert(void *arg)
{
    // Example insert function call
    insert_record((hashRecord **)arg, "testKey", 123);
    return NULL;
}

void test_insert()
{
    hashRecord *table[TABLE_SIZE] = {0};
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_insert, table);
    pthread_create(&thread2, NULL, thread_insert, table);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Check results - maybe print out the table or check specific entries
}

int main()
{
    printf("Running tests...\n");
    test_insert();
    // Call other test functions
    printf("Tests completed.\n");
    return 0;
}
