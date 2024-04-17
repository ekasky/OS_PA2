#include "rwlock.h"
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Jenkins_hash_function
uint32_t jenkins_hash(const char* key) {
	uint32_t hash = 0;
	for (int i = 0; key[i] != '\0'; i++) {
		hash += key[i];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

#define MAPLEN 1
#define NAMELEN 49

typedef struct HashRecord {
	uint32_t hash;
	uint32_t salary;
	struct HashRecord* next;
	char name[NAMELEN + 1];
} HashRecord;

typedef struct HashEntry {
	rwlock_t rwlock;
	HashRecord* record;
} HashEntry;

typedef struct Hashmap {
	HashEntry table[MAPLEN];
} Hashmap;

Hashmap hashmap_init() {
	Hashmap map;

	for (int i = 0; i < MAPLEN; i++) {
		rwlock_init(&map.table[i].rwlock);
		map.table[i].record = NULL;
	}

	return map;
}

/// Returns a uint32_t salary or -1 if not found.
int64_t hashmap_search(Hashmap* hashmap, const char* name) {
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_readlock(&entry->rwlock);

	const HashRecord* record = entry->record;
	int64_t return_value = -1;
	while (record != NULL) {
		if (record->hash == hash) {
			return_value = record->salary;
			break;
		}
		record = record->next;
	}

	rwlock_release_readlock(&entry->rwlock);
	return return_value;
}

void hashmap_insert(Hashmap* hashmap, const char* name, uint32_t salary) {
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_writelock(&entry->rwlock);

	HashRecord** last_ptr = &entry->record;
	HashRecord* record = *last_ptr;
	while (1) {
		if (record == NULL) {
			HashRecord* new = malloc(sizeof(*new));
			*new = (HashRecord){.hash = hash, .salary = salary, .next = NULL};
			strncpy(new->name, name, NAMELEN);
			*last_ptr = new;
			break;
		} else if (record->hash == hash) {
			record->salary = salary;
			break;
		}
		last_ptr = &record->next;
		record = *last_ptr;
	}

	rwlock_release_writelock(&entry->rwlock);
}

void hashmap_delete(Hashmap* hashmap, const char* name) {
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_writelock(&entry->rwlock);

	HashRecord** last_ptr = &entry->record;
	HashRecord* record = *last_ptr;
	while (record != NULL) {
		if (record->hash == hash) {
			*last_ptr = record->next;
			free(record);
			break;
		}
		last_ptr = &record->next;
		record = *last_ptr;
	}

	rwlock_release_writelock(&entry->rwlock);
}

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
