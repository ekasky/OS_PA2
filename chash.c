#include "hashdb.h"
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

Hashmap map;

void* thread(void* arg) {
	char* name = (char*)arg;

	for (int i = 0; i < 1000000; i++) {
		const uint32_t salary = rand();

		hashmap_insert(&map, name, salary);
		int64_t ret = hashmap_search(&map, name);
		hashmap_delete(&map, name);

		assert(ret >= 0);
		assert(ret == salary);
	}

	return NULL;
}

int main() {
	map = hashmap_init();

	pthread_t t1;
	pthread_t t2;
	pthread_create(&t1, NULL, thread, "John");
	pthread_create(&t2, NULL, thread, "Jane");
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}
