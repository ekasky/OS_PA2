#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "includes/rwlocks.h"
#include "includes/hashdb.h"

#define BUFFER_SIZE 255
#define HASH_TABLE_SIZE 10

/* Parser */

typedef struct line_t {

    char* command;
    char* param_one;
    char* param_two;

} line_t;

FILE* open_input_file();
FILE* open_output_file();
line_t parse_line(char* buffer);
void free_line(line_t line);
void* hash_table_thread_function(void* arg);

/* Threads */

typedef struct thread_args_t {

    hash_record_t** hash_table;
    rwlock_t* lock;
    FILE* in_fp;
    FILE* out_fp;

} thread_args_t;

// Create mutex lock for file reading -- I used chatGPT to help fix the input reading within the threads (https://chat.openai.com/share/6276eff2-19a2-4f4c-be44-7f00527a9630)
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_t* allocate_threads(size_t num_threads);


/* Entry Point */

int main(void) {

    // Open input file for reading
    FILE* in_fp = open_input_file();

    // Create or open the output.txt file
    FILE* out_fp = open_output_file();

    // Create a empty hash table
    hash_record_t** hash_table = create_hash_table(HASH_TABLE_SIZE);

    // Create a buffer to store a line read in from file
    char buffer[BUFFER_SIZE];

    // Read in the number of threads needed from the input file
    fgets(buffer, BUFFER_SIZE, in_fp);
    line_t line = parse_line(buffer);
    uint32_t num_threads = atoi(line.param_one);
    free_line(line);
    
    fprintf(out_fp, "Running %d threads\n", num_threads);

    // Create hash table lock to protect reads and writes
    rwlock_t* lock = rwlock_init();

    // Create a variable to store the number of lock acquisitions and releases
    int num_acquisitions = 0;
    int num_releases = 0;

    // Create thread args needed to run
    thread_args_t thread_args = {.hash_table = hash_table, .in_fp = in_fp, .out_fp = out_fp, .lock = lock};

    // Allocate space for the pthreads
    pthread_t* threads = allocate_threads(num_threads);

    // Run the threads
    for(uint32_t i = 0; i < num_threads; i++) {

        pthread_create(&threads[i], NULL, hash_table_thread_function, (void*)&thread_args);

    }

    // Wait for threads to finish
    for(uint32_t i = 0; i < num_threads; i++) {

        pthread_join(threads[i] ,NULL);

    }

    // Close the input file
    fclose(in_fp);

    // Close the output file
    fclose(out_fp);

    // Free the lock
    free(lock);

    // Free the threads
    free(threads);

    // Free the hash table
    free_hash_table(hash_table, HASH_TABLE_SIZE);

    return 0;
}

FILE* open_input_file() {

    FILE* fp = fopen("commands.txt", "r");

    if(!fp) {

        fprintf(stderr, "[ERROR]: Could not open commands.txt file\n");
        exit(1);

    }

    return fp;
 
}

FILE* open_output_file() {

    FILE* fp = fopen("output.txt", "w");

    if(!fp) {

        fprintf(stderr, "[ERROR]: Could not create or open the output.txt file\n");
        exit(1);

    }

    return fp;

}

line_t parse_line(char* buffer) {

    line_t line;

    // Get the command parameter
    char* token = strtok(buffer, ",");

    if(!token) {

        fprintf(stderr, "[ERROR]: Failed to tokenize input\n");
        exit(1);

    }

    line.command = (char*)calloc(strlen(token), sizeof(char));

    if(!line.command) {

        fprintf(stderr, "[ERROR]: Could not allocate space for command\n");
        exit(1);

    }

    strcpy(line.command, token);

    // Get the parameter one
    token = strtok(NULL, ",");

    if(!token) {

        fprintf(stderr, "[ERROR]: Failed to tokenize input\n");
        exit(1);

    }
    
    line.param_one = (char*)calloc(strlen(token), sizeof(char));

    if(!line.param_one) {

        fprintf(stderr, "[ERROR]: Could not allocate space for param_one\n");
        exit(1);

    }

    strcpy(line.param_one, token);

    // Get the parameter two
    token = strtok(NULL, ",");

    if(!token) {

        fprintf(stderr, "[ERROR]: Failed to tokenize input\n");
        exit(1);

    }
    
    line.param_two = (char*)calloc(strlen(token), sizeof(char));

    if(!line.param_two) {

        fprintf(stderr, "[ERROR]: Could not allocate space for param_two\n");
        exit(1);

    }

    strcpy(line.param_two, token);


    return line;

}

void free_line(line_t line) {

    free(line.command);
    free(line.param_one);
    free(line.param_two);

}


pthread_t* allocate_threads(size_t num_threads) {

    pthread_t* threads = (pthread_t*)calloc(num_threads, sizeof(pthread_t));

    if(!threads) {

        fprintf(stderr, "[ERROR]: Could not allocate space for threads\n");
        exit(1);

    }

    return threads;

}

void* hash_table_thread_function(void* arg) {

    // Arguments
    thread_args_t* args = (thread_args_t*)arg;

    // Buffers
    char buffer[BUFFER_SIZE];
    line_t line;

    // Lock the file access -- I used chatGPT to help fix the input reading within the threads (https://chat.openai.com/share/6276eff2-19a2-4f4c-be44-7f00527a9630)
    pthread_mutex_lock(&file_mutex);

    // Read in the next line of the file
    fgets(buffer, BUFFER_SIZE, args->in_fp);
    line = parse_line(buffer);

    // Unlock the file access -- I used chatGPT to help fix the input reading within the threads (https://chat.openai.com/share/6276eff2-19a2-4f4c-be44-7f00527a9630)
    pthread_mutex_unlock(&file_mutex);

    // Perform correct operation

    if( !strcmp(line.command, "insert") ) {
        insert(args->hash_table, HASH_TABLE_SIZE, args->lock, line.param_one, atoi(line.param_two), args->out_fp);
    }

    else if( !strcmp(line.command, "delete") ) {
        delete(args->hash_table, HASH_TABLE_SIZE, args->lock, line.param_one, args->out_fp);
    }

    else if( !strcmp(line.command, "search") ) {
        search(args->hash_table, HASH_TABLE_SIZE, args->lock, line.param_one, args->out_fp);
    }

    else if( !strcmp(line.command, "print") ) {
        print(args->hash_table, HASH_TABLE_SIZE, args->lock, args->out_fp);
    }

    // Free line buffer
    free_line(line);

    return NULL;

}
