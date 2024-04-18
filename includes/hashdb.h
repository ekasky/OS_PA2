#ifndef __HASHDB_H__
#define __HASHDB_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rwlocks.h"

typedef struct hash_struct {
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hash_record_t;

hash_record_t** create_hash_table(size_t table_size);
void free_hash_table(hash_record_t** hash_table, size_t table_size);
uint32_t jenkins_one_at_a_time_hash(const char* key, size_t length);
hash_record_t* create_hash_record(uint32_t hash, char* name, uint32_t salary);
void insert(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, uint32_t value, FILE* fp, int* num_acquisitions, int* num_releases);
void delete(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp, int* num_acquisitions, int* num_releases);
hash_record_t* search(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp, int* num_acquisitions, int* num_releases);
void print(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, FILE* fp, int* num_acquisitions, int* num_releases);
void console_log_hash_table(hash_record_t** hash_table, size_t table_size, rwlock_t* lock);

#endif

