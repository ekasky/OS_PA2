#include "hashdb.h"
#include "rwlock.h"
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR(s) _str(s)
#define _str(s) #s
#define MAXCMDLEN 6
#define NAMELEN 128

typedef enum {
    NONE, PRINT, SEARCH, INSERT, DELETE
} Operation;

typedef struct {
    Hashmap* map;
    FILE* outputLog;
    pthread_t threadId;
    Operation operation;
    char name[NAMELEN + 1];
    uint32_t salary;
} ThreadCommand;

void* processCommand(void* arg) {
    ThreadCommand* cmd = (ThreadCommand*)arg;

    switch (cmd->operation) {
        case PRINT:
        
			// DO PRINT

            break;
        case SEARCH:
            fprintf(cmd->outputLog, "SEARCH,%s\n", cmd->name);
            HashRecord record;
            if (hashmap_search(cmd->map, cmd->name) < 0)
                fputs("No Record Found\n", cmd->outputLog);
            else
                record_print(&record);
            break;
        case INSERT:
            fprintf(cmd->outputLog, "INSERT,%s,%" PRIu32 "\n", cmd->name, cmd->salary);
            hashmap_insert(cmd->map, cmd->name, cmd->salary);
            break;
        case DELETE:
            fprintf(cmd->outputLog, "DELETE,%s\n", cmd->name);
            hashmap_delete(cmd->map, cmd->name);
            break;
        default:
            abort();
    }

    return NULL;
}

int main() {
    FILE* input = fopen("commands.txt", "r");
    if (!input) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    FILE* outputLog = fopen("output.txt", "w");
    if (!outputLog) {
        perror("Failed to open log file");
        fclose(input);
        return EXIT_FAILURE;
    }

    unsigned int threadCount;
    if (fscanf(input, "threads,%u,0 ", &threadCount) <= 0) {
        fprintf(stderr, "Error reading thread count\n");
        fclose(input);
        fclose(outputLog);
        return EXIT_FAILURE;
    }

    ThreadCommand* commands = malloc(sizeof(ThreadCommand) * threadCount);
    if (!commands) {
        perror("Failed to allocate memory for commands");
        fclose(input);
        fclose(outputLog);
        return EXIT_FAILURE;
    }

    Hashmap mapVar = hashmap_init();
	Hashmap* map = &mapVar;
    ThreadCommand baseCmd = {.map = map, .outputLog = outputLog};

    for (unsigned int i = 0; i < threadCount; i++) {
        ThreadCommand* cmd = &commands[i];
        *cmd = baseCmd;

        char opStr[MAXCMDLEN + 1];
        if (fscanf(input, "%" STR(MAXCMDLEN) "[^,],%" STR(NAMELEN) "[^,],%" PRIu32 " ", opStr, cmd->name, &cmd->salary) <= 0) {
            fprintf(stderr, "Error reading commands\n");
            free(commands);
            fclose(input);
            fclose(outputLog);
            return EXIT_FAILURE;
        }

        cmd->operation = stringToOperation(opStr);
        assert(pthread_create(&cmd->threadId, NULL, processCommand, cmd) == 0);
    }

    for (unsigned int i = 0; i < threadCount; i++) {
        pthread_join(commands[i].threadId, NULL);
    }

    fprintf(outputLog, "\nFinal Table:\n");
    hashmap_print(map, 0);

    fclose(input);
    fclose(outputLog);
    free(commands);	
	

	//free hashmaps

    return 0;
}
