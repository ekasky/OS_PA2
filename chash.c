#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include "./includes/hashdb.h"
#include "./includes/common.h"
#include "./includes/common_threads.h"

#define LINE_BUFFER_SIZE 255
#define HASH_TABLE_SIZE 10

/* Define data structs */

typedef struct Line {

	char* command;
	char* param_one;
	char* param_two;

} Line;

/* Function Prototypes */
FILE* open_file();
int read_line(FILE* fp, char* buffer, size_t bufferSize);
Line* parse_line(char* buffer);
void free_parsed_line(Line* line);
pthread_t* allocate_threads(size_t num_threads);

int main(void) {

	char buffer[LINE_BUFFER_SIZE];
	Line* temp_line = NULL;
	uint32_t num_threads = 0;
	hash_record_t** ht = create_hash_table(HASH_TABLE_SIZE);
	rwlock_t* lock = rwlock_init();

	// Open the commands file in read mode
	FILE* fp = open_file();
	
	// Allocate space for the threads
	read_line(fp, buffer, LINE_BUFFER_SIZE);
	temp_line = parse_line(buffer);
	num_threads = atoi(temp_line->param_one);
	free_parsed_line(temp_line);

	pthread_t* threads = allocate_threads(num_threads);

	// Create the threads
	for(uint32_t i = 0; i < num_threads; i++) {

		

	}

	/*
	// Loop through each line of the file
	while(read_line(fp, buffer, LINE_BUFFER_SIZE)) {
		
		// Extract the parameters from the line
		temp_line = parse_line(buffer);
		
		// Run the command read from the line
		if( strcmp(temp_line->command, "insert") == 0 ) {

			insert(ht, HASH_TABLE_SIZE, lock, temp_line->param_one, atoi(temp_line->param_two));

		} else if( strcmp(temp_line->command, "print") == 0 ) {

			print_hash_table_console(ht, HASH_TABLE_SIZE);

		} else if( strcmp(temp_line->command, "delete") == 0 ) {

			delete(ht, HASH_TABLE_SIZE, lock, temp_line->param_one);

		} else if( strcmp(temp_line->command, "search") == 0 ) {

			hash_record_t* find = search(ht, HASH_TABLE_SIZE, lock, temp_line->param_one);

			if(!find) printf("NULL\n");
			else printf("FOUND: %s\n", find->name);

		}

		free_parsed_line(temp_line);	
		
	}
	*/
	
	// Close the commands.txt file, free the lock, free the threads, and free the hash table
	fclose(fp);
	free(lock);
	free(threads);
	destory_hash_table(ht, HASH_TABLE_SIZE);

	return 0;
}



/* Function Definitions */

FILE* open_file() {

	// Attempt to open the file in read mode (commads.txt)
	FILE* fp = fopen("commands.txt", "r");
	
	// If the file cannot be open for found print a error and return a NULL ptr
	if(fp == NULL) {
	
		perror("[ERROR]: Could not open 'commands.txt'");
		exit(1);
	
	}
	
	return fp;
	

}

int read_line(FILE* fp, char* buffer, size_t bufferSize) {

	if( fgets(buffer, bufferSize, fp) != NULL ) {
		return 1;			// Line read sucessfully
	}
	
	else {
		return 0;			// EOF encountered or error
	}
	
}


Line* parse_line(char* buffer) {

	// Remove the new line character from the end of the line
	buffer[strcspn(buffer, "\n")] = '\0';					// This line was A.I Generated

	// Allocate memory for the line
	Line* line = (Line*)malloc(sizeof(Line));
	
	// Print a error and exit if there is a memory allocation error
	if(line == NULL) {
	
		perror("[ERROR]: Memory allocation error");
		exit(1);
	
	}
	
	// Tokenize the line by commands
	char* token = strtok(buffer, ",");
	
	if(token == NULL) {
	
		perror("[ERROR]: Could not tokenize string");
		exit(1);
	
	}
	
	// Copy the first token to the command
	line->command = strdup(token);		// This line used A.I to figure out how to allocate string effincently 
	
	
	// Get the next parameter (token 2)
	
	token = strtok(NULL, ",");
	
	if(token == NULL) {
	
		perror("[ERROR]: Could not tokenize string");
		exit(1);
	
	}
	
	// Copy the second token to the parameter_one
	line->param_one = strdup(token);	// This line used A.I to figure out how to allocate string effincently
	
	// Get the next parameter (token 3)
	
	token = strtok(NULL, ",");
	
	if(token == NULL) {
	
		perror("[ERROR]: Could not tokenize string");
		exit(1);
	
	}
	
	// Copy the second token to the parameter_one
	line->param_two = strdup(token);	// This line used A.I to figure out how to allocate string effincently
	
	return line;

}

void free_parsed_line(Line* line) {

	if(line == NULL) return;

	free(line->command);
	free(line->param_one);
	free(line->param_two);

	free(line);

}

pthread_t* allocate_threads(size_t num_threads) {

	pthread_t* threads = (pthread_t*)calloc(num_threads, sizeof(pthread_t));

	if(!threads) {

		fprintf(stderr, "[ERROR]: Could not allocate space for pthreads\n");
		exit(1);

	}

	return threads;

}