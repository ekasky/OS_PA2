#include "../includes/hashdb.h"

/* https://en.wikipedia.org/wiki/Jenkins_hash_function */
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

hashRecord** create_hash_table(size_t table_size) {

	hashRecord** ht = (hashRecord**)calloc(table_size, sizeof(hashRecord*));
	
	if(!ht) {
	
		fprintf(stderr, "[ERROR]: Could not allocate space for hash table\n");
		exit(1);
	
	}
	
	for(uint32_t i = 0; i < table_size; i++) {
	
		ht[i] = NULL;
	
	}

	return ht;
}

void destroy_hash_table(hashRecord** ht, size_t table_size) {

	if(ht == NULL) return;					// Nothing to do hash table is NULL
	
	for(uint32_t i = 0; i < table_size; i++) {
	
		hashRecord* temp = ht[i];
		hashRecord* prev = NULL;
		
		while(temp) {
		
			prev = temp;
			temp = temp->next;
			free(prev);
		
		}
	
	}
	
	free(ht);

}
