#include "rwlock.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

#define MAPLEN 64

typedef struct HashRecord {
	uint32_t hash;
	uint32_t salary;
	struct HashRecord* next;
	char name[50];
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

uint32_t hashmap_search(Hashmap* hashmap, const char* name) {
	uint32_t hash = jenkins_hash(name);
	// todo
}

int main() {
	uint32_t hash = jenkins_hash("The quick brown fox jumps over the lazy dog");
	printf("%x\n", hash);
}
