#include "hashdb.h"
#include "rwlock.h"
#include <stdlib.h>
#include <string.h>

// Used to stringify defined numbers
#define STR(s) _str(s)
#define _str(s) #s

void record_print(const HashRecord* record) {
	fprintf(
		LOG_OUTPUT,
		"%" PRIu32 ",%s,%" PRIu32 "\n",
		record->hash,
		record->name,
		record->salary
	);
}

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

Hashmap hashmap_init() {
	Hashmap map;

	for (int i = 0; i < MAPLEN; i++) {
		rwlock_init(&map.table[i].rwlock);
		map.table[i].record = NULL;
	}

	return map;
}

// Returns -1 if not found.
int hashmap_search(Hashmap* hashmap, const char* name, HashRecord* out) {
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_readlock(&entry->rwlock);

	const HashRecord* record = entry->record;
	_Bool found = 0;
	while (record != NULL) {
		if (record->hash == hash) {
			*out = *record;
			out->next = NULL;
			found = 1;
			break;
		}
		record = record->next;
	}

	rwlock_release_readlock(&entry->rwlock);
	return found ? 0 : -1;
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

void hashmap_print(Hashmap* hashmap, _Bool lock) {
	for (int i = 0; i < MAPLEN; i++) {
		HashEntry* entry = &hashmap->table[i];

		// Unsynchronized/nonatomic initial check. Races and tears are okay
		// because we check again after locking.
		if (entry->record == NULL)
			continue;

		if (lock)
			rwlock_acquire_readlock(&entry->rwlock);

		for (const HashRecord* record = entry->record; record != NULL;
			 record = record->next) {
			record_print(record);
		}

		if (lock)
			rwlock_release_readlock(&entry->rwlock);
	}
}
