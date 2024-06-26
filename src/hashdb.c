#include "../includes/hashdb.h"

/* Instantiating hash table */
hash_record_t** create_hash_table(size_t table_size) {

    /* Allocate enough memory */
    hash_record_t** hash_table = (hash_record_t**)calloc(table_size, sizeof(hash_record_t*));

    if(!hash_table) {

        fprintf(stderr, "[ERROR]: Could not allocate space for hash table\n");
        exit(1);

    }

    /* Initialize that memory to null */
    for(uint32_t i = 0; i < table_size; i++) {

        hash_table[i] = NULL;

    }

    return hash_table;

}

/* Free the entire hash table */
void free_hash_table(hash_record_t** hash_table, size_t table_size) {
    
    /* For each bucket free the items inside */
    for(uint32_t i = 0; i < table_size; i++) {

        hash_record_t* temp = hash_table[i];
        hash_record_t* prev = NULL;

        while(temp) {

            prev = temp;
            temp = temp->next;
            free(prev);

        }

    }

    /* Free list of null buckets */
    free(hash_table);

}

/* Hashing function from https://en.wikipedia.org/wiki/Jenkins_hash_function */
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

/* Returns a hash record that can be stored inside the hash map */
hash_record_t* create_hash_record(uint32_t hash, char* name, uint32_t salary) {

    /* Allocate the memory */
    hash_record_t* record = (hash_record_t*)calloc(1, sizeof(hash_record_t));

    if(!record) {

        fprintf(stderr, "[ERROR]: Could not allocate space for record\n");
        exit(1);

    }

    /* Assign appropriate values */
    record->hash = hash;
    strcpy(record->name, name);
    record->salary = salary;
    record->next = NULL;

    return record;

}

/* Insertion into hash map function */
void insert(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, uint32_t value, FILE* fp, _Atomic int* num_acquisitions, _Atomic int* num_releases) {

    /* Compute the hash value */
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));

    fprintf(fp, "INSERT, %u, %s, %u\n", hash, key, value);

    /* Acquire writing lock */
    rwlock_acquire_write_lock(lock, fp, num_acquisitions);

    /* Create a reference to the correct bucket corresponsing to the hash */
    hash_record_t* bucket_head = hash_table[hash % table_size];

    /* Insert the new record into the bucket if bucket is empty */
    if(!bucket_head) {

        hash_table[hash % table_size] = create_hash_record(hash, key, value);
       	
        rwlock_release_write_lock(lock, fp, num_releases);

        return;

    }

    /* If not empty we need to find the next spot available, sorted by hash */
    hash_record_t* temp = bucket_head;
    hash_record_t* prev = NULL;

    while(temp && temp->hash < hash) {
        prev = temp;
        temp = temp->next;
    }

    hash_record_t* record = create_hash_record(hash, key, value);

    /* Insertion based on where it belong with sort */
    if(!prev) {

        record->next = bucket_head;
        hash_table[hash % table_size] = record;

    }
    else if(!prev->next) {

        prev->next = record;

    }
    else {

        record->next = prev->next;
        prev->next = record;

    }

    /* Releasing our write lock */
    rwlock_release_write_lock(lock, fp, num_releases);

}

/* Deletion from hash map function */
void delete(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp, _Atomic int* num_acquisitions, _Atomic int* num_releases) {

    fprintf(fp, "DELETE, %s\n", key);

    /* Aquire the write lock */
    rwlock_acquire_write_lock(lock, fp, num_acquisitions);

    /* Compute the hash */
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));
    
    /* Find the record in the bucket */
    hash_record_t* temp = hash_table[hash % table_size];
    hash_record_t* prev = NULL;
    
    while(temp) {
    
    	if( !strcmp(temp->name, key) ) break;
    	prev = temp;
    	temp = temp->next;
    
    }
    
    /* Remove the record from the bucket if found */
    if(!temp) {
    
    	rwlock_release_write_lock(lock, fp, num_releases);
    	return;
    
    }
    
    if(temp == hash_table[hash % table_size]) {
    
    	free(temp);
    	hash_table[hash % table_size] = NULL;
    	rwlock_release_write_lock(lock, fp, num_releases);
    	return;
    
    }
    
    if(!temp->next) {
    
    	prev->next = NULL;
    	free(temp);
    	rwlock_release_write_lock(lock, fp, num_releases);
    	return;
    
    }
    
    prev->next = temp->next;
    temp->next = NULL;
    free(temp);
    
    /* Releasing lock*/
    rwlock_release_write_lock(lock, fp, num_releases);
    

}

/* Searching for record in hashmap function */
hash_record_t* search(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, char* key, FILE* fp, _Atomic int* num_acquisitions, _Atomic int* num_releases) {

    fprintf(fp, "SEARCH, %s\n", key);

    /* Aquire the read lock */
    rwlock_acquire_read_lock(lock, fp, num_acquisitions);

    /* Compute the hash */
    uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key));

    /* Create a refrence to the bucket */
    hash_record_t* bucket_head = hash_table[hash % table_size];

    /* Find the record in the table */
    if(!bucket_head) {

        rwlock_release_read_lock(lock, fp, num_releases);
        return NULL;

    }

    /* Traversal to find record */
    hash_record_t* temp = bucket_head;

    while(temp) {

        if( !strcmp(temp->name, key) ) break;
        temp = temp->next;

    }

    /* If not found we return null */
    if(!temp) {

        rwlock_release_read_lock(lock, fp, num_releases);
        return NULL;

    }
    
    /* Release the read lock*/
    rwlock_release_read_lock(lock, fp, num_releases);

    return temp;

}

/* Function to print the hashmap */
void print(hash_record_t** hash_table, size_t table_size, rwlock_t* lock, FILE* fp, _Atomic int* num_acquisitions, _Atomic int* num_releases) {

    /* Aquire the read lock */
    rwlock_acquire_read_lock(lock, fp, num_acquisitions);

    /* Create one sorted linked list based on hash values */
    hash_record_t* head = NULL;

    hash_record_t* temp = NULL;
    hash_record_t* prev = NULL;

    for(uint32_t i = 0; i < table_size; i++) {

        temp = hash_table[i];

        while(temp) {

            /* If the linked list is empty insert to the head */
            if(!head) {                                 
                head = create_hash_record(temp->hash, temp->name, temp->salary);
            }
            /* If the current value going into the list is less than the head hash insert to the front */
            else if(temp->hash < head->hash) {          

                hash_record_t* record = create_hash_record(temp->hash, temp->name, temp->salary);
                record->next = head;
                head = record;

            }
            /* Otherwise search for the appropriate location in the linked list */
            else {

                hash_record_t* record = create_hash_record(temp->hash, temp->name, temp->salary);
                hash_record_t* current = head;
                prev = NULL;

                /* Insert the record in sorted order */
                while(current) {

                    if(current->hash < record->hash) {
                        prev = current;
                        current = current->next;
                        continue;
                    }
                    else {

                        prev->next = record;
                        record->next = current;
                        break;

                    }

                }

                if(!current) {
                    prev->next = record;
                }

            }

            temp = temp->next;

        }

    }

    /* Print the sorted linked list */
    temp = head;

    while(temp) {

        fprintf(fp, "%u,%s,%u\n", temp->hash, temp->name, temp->salary);
        temp = temp->next;

    }

    /* Free the linked list created for printing */
    temp = head;
    prev = NULL;

    while(temp) {

        prev = temp;
        temp = temp->next;
        free(prev);

    }

    /* Release the read lock */
    rwlock_release_read_lock(lock, fp, num_releases);

}
