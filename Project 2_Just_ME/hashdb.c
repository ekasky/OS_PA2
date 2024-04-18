#include "hashdb.h"
#include "rwlocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *out; // Ensure that this is declared in chash.c or another relevant part of your program

// Jenkins one-at-a-time hash function
uint32_t jenkins_one_at_a_time_hash(const uint8_t *key, size_t length)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != length)
    {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

// Insert a record into the hash table
void insert_record(hashRecord **table, const char *name, uint32_t salary)
{
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));
    int index = hash % TABLE_SIZE;
    hashRecord *newNode = (hashRecord *)malloc(sizeof(hashRecord));
    if (newNode == NULL)
    {
        fprintf(out, "Failed to allocate memory for new record.\n");
        return;
    }

    newNode->hash = hash;
    strcpy(newNode->name, name);
    newNode->salary = salary;
    newNode->next = NULL;

    rwlock_acquire_writelock(&lock);
    fprintf(out, "WRITE LOCK ACQUIRED\n");

    // Insert node at the beginning of the linked list
    newNode->next = table[index];
    table[index] = newNode;

    fprintf(out, "INSERT,%u,%s,%u\n", hash, name, salary);

    rwlock_release_writelock(&lock);
    fprintf(out, "WRITE LOCK RELEASED\n");
}

// Delete a record from the hash table
void delete_record(hashRecord **table, const char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));
    int index = hash % TABLE_SIZE;

    rwlock_acquire_writelock(&lock);
    fprintf(out, "WRITE LOCK ACQUIRED\n");

    hashRecord *current = table[index], *prev = NULL;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            if (prev == NULL)
            {
                table[index] = current->next;
            }
            else
            {
                prev->next = current->next;
            }
            free(current);
            fprintf(out, "DELETE,%s\n", name);
            break;
        }
        prev = current;
        current = current->next;
    }

    rwlock_release_writelock(&lock);
    fprintf(out, "WRITE LOCK RELEASED\n");
}

/// Search for a record in the hash table
hashRecord *search_record(hashRecord **table, const char *name)
{
    uint32_t hash = jenkins_one_at_a_time_hash((const uint8_t *)name, strlen(name));
    int index = hash % TABLE_SIZE;

    rwlock_acquire_readlock(&lock);
    fprintf(out, "READ LOCK ACQUIRED\n");

    hashRecord *current = table[index];
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            fprintf(out, "%u,%s,%u\n", current->hash, current->name, current->salary);
            rwlock_release_readlock(&lock);
            fprintf(out, "READ LOCK RELEASED\n");
            return current; // Return the found record
        }
        current = current->next;
    }

    if (current == NULL)
    {
        fprintf(out, "SEARCH,%s NOT FOUND\n", name);
    }

    rwlock_release_readlock(&lock);
    fprintf(out, "READ LOCK RELEASED\n");
    return NULL; // Return NULL if no record is found
}

// Print the hash table contents
void print_table(FILE *out)
{
    rwlock_acquire_readlock(&lock);
    fprintf(out, "READ LOCK ACQUIRED\n");
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (hashRecord *node = table[i]; node != NULL; node = node->next)
        {
            fprintf(out, "%u,%s,%u\n", node->hash, node->name, node->salary);
        }
    }
    rwlock_release_readlock(&lock);
    fprintf(out, "READ LOCK RELEASED\n");
}
