#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include "./includes/hashdb.h"
#include "./includes/rwlocks.h"

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

int main(void) {

	const size_t bufferSize = 100;
	char buffer[bufferSize];
	Line* temp_line = NULL;
	uint32_t i = 0;
	uint32_t num_threads = 0;
	hashRecord* head = NULL;					// Point to the first node in the hash table list

	// Open the commands file in read mode
	FILE* fp = open_file();
	
	
	// Loop through each line of the file
	while(read_line(fp, buffer, bufferSize)) {
		
		// Extract the parameters from the line
		temp_line = parse_line(buffer);
		
		// Run the command read from the line
		if( strcmp(temp_line->command, "threads") == 0 ) {
			num_threads = atoi(temp_line->param_one);		// Convert the number of threads to a int
		}	
		
	}
	
	// Testing (Will be removed)
	printf("Number of threads: %d\n", num_threads);
	printf("Hash: %x\n", jenkins_one_at_a_time_hash("a", 1));
	printf("Hash: %x\n", jenkins_one_at_a_time_hash("Testing the hash function", 25));
	
	rwlock_t lock;
	rwlock_init(&lock);
	pthread_t readers[3], writers[3];
	for(i = 0; i < 3; i++) {
	
		pthread_create(&readers[i], NULL, reader, &lock);
		pthread_create(&writers[i], NULL, writer, &lock);
	
	}
	
	for(i = 0; i < 3; i++) {
	
		pthread_join(&readers[i], NULL);
		pthread_join(&writers[i], NULL);
	
	}
	
	// Close the commands.txt file
	fclose(fp);

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







