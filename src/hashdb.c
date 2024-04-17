#include "../includes/hashdb.h"

hash_record_t** create_hash_table(size_t table_size) {

    hash_record_t** hash_table = (hash_record_t**)calloc(table_size, sizeof(hash_record_t*));

    if(!hash_table) {

        fprintf(stderr, "[ERROR]: Could not allocate space for hash table\n");
        exit(1);

    }

    for(uint32_t i = 0; i < table_size; i++) {

        hash_table[i] = NULL;

    }

    return hash_table;

}

void free_hash_table(hash_record_t** hash_table, size_t table_size) {

    for(uint32_t i = 0; i < table_size; i++) {

        hash_record_t* temp = hash_table[i];
        hash_record_t* prev = NULL;

        while(temp) {

            prev = temp;
            temp = temp->next;
            free(prev);

        }

    }

    free(hash_table);

}

uint32_t jenkins_one_at_a_time_hash(const char* key, size_t length) {

    size_t i = 0;
    uint32_t hash = 0;

    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;

}

hash_record_t* create_hash_record(uint32_t hash, char* name, uint32_t salary) {

    hash_record_t* record = (hash_record_t*)calloc(1, sizeof(hash_record_t));

    if(!record) {

        fprintf(stderr, "[ERROR]: Could not allocate space for record\n");
        exit(1);

    }

    record->hash = hash;
    strcpy(record->name, name);
    record->salary = salary;
    record->next = NULL;

    return record;

}

void insert(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, uint32_t value, FILE* fp) {

    // Aquire the write lock
    rwlock_acquire_write_lock(lock, fp);

    // Compute the hash value
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));

    // Create a reference to the correct bucket corresponsing to the hash
    hash_record_t* bucket_head = hash_table[hash % table_size];

    // Insert the new record into the bucket

    if(!bucket_head) {

        bucket_head = create_hash_record(hash, key, value);
        
        fprintf(fp, "INSERT, %s, %d\n", key, value);

        rwlock_release_write_lock(lock, fp);

        return;

    }

    hash_record_t* record = create_hash_record(hash, key, value);
    
    record->next = bucket_head;
    bucket_head = record;

    fprintf(fp, "INSERT, %s, %d\n", key, value);

    rwlock_release_write_lock(lock, fp);



}

void delete(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp) {

    // Aquire the write lock
    rwlock_acquire_write_lock(lock, fp);

    // Compute the hash
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));

    // Create a reference to the correct bucket
    hash_record_t* bucket_head = hash_table[hash % table_size];

    // Remove node from the table

    if(!bucket_head) {
        
        rwlock_release_write_lock(lock, fp);
        return;

    }

    hash_record_t* temp = bucket_head;
    hash_record_t* prev = NULL;

    while(temp) {

        if( !strcmp(temp->name, key) ) break;
        prev = temp;
        temp = temp->next;

    }

    if(!temp) {

        rwlock_release_write_lock(lock, fp);
        return;

    }

    if(!temp->next) {

        free(temp);
        prev->next = NULL;

        fprintf(fp, "DELETE, %s", key);
        rwlock_release_write_lock(lock, fp);
        return;

    }

    prev->next = temp->next;
    temp->next = NULL;

    free(temp);

    fprintf(fp, "DELETE, %s", key);
    rwlock_release_write_lock(lock, fp);

}

hash_record_t* search(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp) {

    // Aquire the read lock
    rwlock_acquire_read_lock(lock, fp);

    // Compute the hash
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));

    // Create a refrence to the bucket
    hash_record_t* bucket_head = hash_table[hash % table_size];

    // Find the record in the table
    if(!bucket_head) {

        rwlock_release_read_lock(lock, fp);
        return NULL;

    }

    hash_record_t* temp = bucket_head;

    while(temp) {

        if( !strcmp(temp->name, key) ) break;
        temp = temp->next;

    }

    if(!temp) {

        rwlock_release_read_lock(lock, fp);
        return NULL;

    }

    fprintf(fp, "SEARCH, %s\n", key);
    rwlock_release_read_lock(lock, fp);

    return temp;

}

void print(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, FILE* fp) {

    // Aquire the read lock
    rwlock_acquire_read_lock(lock, fp);

    for(uint32_t i = 0; i < table_size; i++) {

        hash_record_t* temp = hash_table[i];

        while(temp) {

            fprintf(fp, "%d,%s,%d\n", temp->hash, temp->name, temp->salary);
            temp = temp->next;

        }

    }

    rwlock_release_read_lock(lock, fp);

}
