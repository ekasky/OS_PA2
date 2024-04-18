#include "hashdb.h"
#include "rwlocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct
{
    char type[10];
    char name[50];
    uint32_t salary;
} Command;

hashRecord *table[TABLE_SIZE] = {0};
rwlock_t lock;
void *execute_command(void *arg);
void print_table(FILE *out);

extern hashRecord *table[TABLE_SIZE]; // Declared in hashdb.h
extern rwlock_t lock;                 // Declared in hashdb.h
FILE *out;                            // For output log to file

int main()
{
    rwlock_init(&lock);

    FILE *file = fopen("commands.txt", "r");
    if (!file)
    {
        perror("Failed to open commands.txt");
        return EXIT_FAILURE;
    }

    out = fopen("output.txt", "w");
    if (!out)
    {
        perror("Failed to open output.txt");
        fclose(file);
        return EXIT_FAILURE;
    }

    char line[256];
    int thread_count = 0;
    pthread_t threads[256]; // Array to hold thread IDs
    Command commands[256];  // Array to hold commands

    fgets(line, sizeof(line), file); // Read the first line to get the number of threads
    sscanf(line, "threads,%d", &thread_count);
    fprintf(out, "Running %d threads\n", thread_count);

    int i = 0;
    while (fgets(line, sizeof(line), file) && i < thread_count)
    {
        sscanf(line, "%[^,],%[^,],%d", commands[i].type, commands[i].name, &commands[i].salary);
        pthread_create(&threads[i], NULL, execute_command, &commands[i]);
        i++;
    }

    for (i = 0; i < thread_count; i++)
    {
        pthread_join(threads[i], NULL); // Wait for all threads to finish
    }

    print_table(out); // Print the final state of the hash table
    fclose(out);      // Close the output file
    fclose(file);     // Close the input file

    return 0;
}

void *execute_command(void *arg)
{
    Command *cmd = (Command *)arg;
    if (strcmp(cmd->type, "insert") == 0)
    {
        fprintf(out, "INSERT,%s,%u\n", cmd->name, cmd->salary);
        insert_record(table, cmd->name, cmd->salary);
    }
    else if (strcmp(cmd->type, "delete") == 0)
    {
        fprintf(out, "DELETE,%s\n", cmd->name);
        delete_record(table, cmd->name);
    }
    else if (strcmp(cmd->type, "search") == 0)
    {
        fprintf(out, "SEARCH,%s\n", cmd->name);
        search_record(table, cmd->name);
    }
    else if (strcmp(cmd->type, "print") == 0)
    {
        print_table(out);
    }
    return NULL;
}
