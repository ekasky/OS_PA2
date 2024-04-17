#pragma once
#include "rwlock.h"
#include <inttypes.h>

#define MAPLEN 64
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

uint32_t jenkins_hash(const char* key);
Hashmap hashmap_init();
int64_t hashmap_search(Hashmap* hashmap, const char* name);
void hashmap_insert(Hashmap* hashmap, const char* name, uint32_t salary);
void hashmap_delete(Hashmap* hashmap, const char* name);
