#include <stdio.h>
#include <stdlib.h>

/* Define data structs */


/* Function Prototypes */
FILE* open_file();
int read_line(FILE* fp, char* buffer, size_t bufferSize);

int main(void) {

	const size_t bufferSize = 100;
	char buffer[bufferSize];

	// Open the commands file in read mode
	FILE* fp = open_file();
	
	
	// Loop through each line of the file
	while(read_line(fp, buffer, bufferSize)) {
		
		printf("%s", buffer);
	
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

