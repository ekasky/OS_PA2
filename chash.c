#include "rwlock.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

int main() {
	uint32_t hash = jenkins_hash("The quick brown fox jumps over the lazy dog");
	printf("%x\n", hash);
}
