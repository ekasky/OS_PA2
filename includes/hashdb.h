#ifndef __HASHDB_H__
#define __HASHDB_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "rwlocks.h"

typedef struct hash_struct {
	uint32_t hash;
	char name[50];
	uint32_t salary;
	struct hash_struct *next;
} hash_record_t;

/* Function prototypes */
uint32_t jenkins_one_at_a_time_hash(const char* key, size_t length, size_t table_size);
hash_record_t* new_hash_record(uint32_t hash, char* name, uint32_t salary);
hash_record_t** create_hash_table(size_t hash_table_size);
void destory_hash_table(hash_record_t** ht, size_t hash_table_size);
void insert(hash_record_t** ht, size_t hash_table_size, rwlock_t* lock, char* key, uint32_t value);

#endif
