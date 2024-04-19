#include "hashdb.h"
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


// Initalizing hashmap function
Hashmap hashmap_init() {
	Hashmap map;

	// Sets all records to null
	for (int i = 0; i < MAPLEN; i++) {
		rwlock_init(&map.table[i].rwlock);
		map.table[i].record = NULL;
	}

	return map;
}


/// Returns a uint32_t salary or -1 if not found.
int64_t hashmap_search(Hashmap* hashmap, const char* name) {

	// Calculating hash value for key and index in hashmap
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	// Accessing that index in the hashmap and getting the read lock
	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_readlock(&entry->rwlock);

	// Now accessing the record in the hashmap, grabbing value if there, if not its -1
	const HashRecord* record = entry->record;
	int64_t return_value = -1;
	while (record != NULL) {
		if (record->hash == hash) {
			return_value = record->salary;
			break;
		}
		record = record->next;
	}

	// Releasing the readlock and retrieving value
	rwlock_release_readlock(&entry->rwlock);
	return return_value;
}


// Inserting a new key-data pair into hash table
void hashmap_insert(Hashmap* hashmap, const char* name, uint32_t salary) {

	// Calculating the hash of the name and finding the index to insert into hash map
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;	

	// Going into that index and assigning a write lock
	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_writelock(&entry->rwlock);

	// Finding the end of the linked list so we can append new value
	HashRecord** last_ptr = &entry->record;
	HashRecord* record = *last_ptr;
	while (1) {
		// If we reach end of list and name not found, append it
		if (record == NULL) {
			HashRecord* new = malloc(sizeof(*new));
			*new = (HashRecord){.hash = hash, .salary = salary, .next = NULL};
			strncpy(new->name, name, NAMELEN);
			*last_ptr = new;
			break;
		// If we find the name in the list, modify values based on input
		} else if (record->hash == hash) {
			record->salary = salary;
			break;
		}
		// Traverse the list
		last_ptr = &record->next;
		record = *last_ptr;
	}

	// Releasing our writelock
	rwlock_release_writelock(&entry->rwlock);
}


// Deletion of key-data pair in hash table
void hashmap_delete(Hashmap* hashmap, const char* name) {

	// Calculating key value as hash and index in hash table
	uint32_t hash = jenkins_hash(name);
	int index = hash % MAPLEN;

	// Grabbing the records at that index and acquiring writelock
	HashEntry* entry = &hashmap->table[index];
	rwlock_acquire_writelock(&entry->rwlock);

	// Traversing the records to delete the desired record
	HashRecord** last_ptr = &entry->record;
	HashRecord* record = *last_ptr;
	while (record != NULL) {
		// If record found, we will remove it from the list
		if (record->hash == hash) {
			*last_ptr = record->next;
			free(record);
			break;
		}
		// Traverse
		last_ptr = &record->next;
		record = *last_ptr;
	}
	// If record is not found nothing occurs

	// Releasing the writelock that was acquired
	rwlock_release_writelock(&entry->rwlock);
}
