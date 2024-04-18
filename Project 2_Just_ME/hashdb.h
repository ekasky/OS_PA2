#ifndef __HASHDB_H__
#define __HASHDB_H__

#include <stdint.h>
#include <stdio.h> // Include this to define FILE
#include "rwlocks.h"

#define TABLE_SIZE 1021 // Define the size of the hash table as a prime number

// Node structure for the hash table
typedef struct hash_struct
{
    uint32_t hash;            // Hash value computed by the hash function
    char name[50];            // Key - Name of up to 50 characters
    uint32_t salary;          // Value - Associated salary
    struct hash_struct *next; // Pointer to the next node in the list
} hashRecord;

// Global variables
extern hashRecord *table[TABLE_SIZE];
extern rwlock_t lock;
extern FILE *out; // Output file handle, used for logging

// Function prototypes
uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length);
void insert_record(hashRecord **table, const char *name, uint32_t salary);
hashRecord *search_record(hashRecord **table, const char *name);
void delete_record(hashRecord **table, const char *name);
void print_table(FILE *out);

#endif // __HASHDB_H__
