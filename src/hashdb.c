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