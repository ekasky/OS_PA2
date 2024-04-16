#include "../includes/hashdb.h"

/* https://en.wikipedia.org/wiki/Jenkins_hash_function */
uint32_t jenkins_one_at_a_time_hash(const char* key, size_t length, size_t table_size) {

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
	
	return hash % table_size;
	
}

hash_record_t* new_hash_record(uint32_t hash, char* name, uint32_t salary) {

	hash_record_t* hr = (hash_record_t*)calloc(1, sizeof(hash_record_t));

	if(!hr) {

		fprintf(stderr, "[ERROR]: Could not allocate space for hash record\n");
		exit(1);

	}

	hr->hash = hash;
	strcpy(hr->name, name);
	hr->salary = salary;
	hr->next = NULL;

	return hr;

}

hash_record_t** create_hash_table(size_t hash_table_size) {

	hash_record_t** ht = (hash_record_t**)calloc(hash_table_size, sizeof(hash_record_t*));

	if(!ht) {

		fprintf(stderr, "[ERROR]: Could not allocate space for hash table\n");
		exit(1);

	}

	for(uint32_t i = 0; i < hash_table_size; i++) {

		ht[i] = NULL;

	}

	return ht;

}

void destory_hash_table(hash_record_t** ht, size_t hash_table_size) {

	if(!ht) return;

	for(uint32_t i = 0; i < hash_table_size; i++) {

		hash_record_t* temp = ht[i];
		hash_record_t* prev = NULL;

		while(temp) {

			prev = temp;
			temp = temp->next;
			free(prev);

		}

	}

	free(ht);

}

void insert(hash_record_t** ht, size_t hash_table_size, rwlock_t* lock, char* key, uint32_t value) {

	// Aquire the write lock
	//rwlock_acquire_write_lock(lock);

	// Compute the hash
	uint32_t hash = jenkins_one_at_a_time_hash(key, strlen(key), hash_table_size);
	hash_record_t** bucket = &ht[hash];

	// Insert to bucket

	if(!(*bucket)) {

		*bucket = new_hash_record(hash, key, value);
		return;

	}

	hash_record_t* hr = new_hash_record(hash, key, value);
	hr->next = *bucket;
	*bucket = hr;

	// Relase the write lock
	//rwlock_release_write_lock(lock);

}

void print_hash_table_console(hash_record_t** ht, size_t hash_table_size) {

	if(ht == NULL) {
		printf("TABLE IS NULL\n");
	}

	printf("Hash Table\n");
	printf("___________\n");

	for(uint32_t i = 0; i < hash_table_size; i++) {

		hash_record_t* bucket = ht[i];

		printf("%.3d: ", i);
		while(bucket) {

			printf("%s -> ", bucket->name);
			bucket = bucket->next;

		}

		printf("\n");

	}

	printf("\n\n");

}