#include "hashdb.h"
#include "rwlock.h"
#include <assert.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Used to stringify defined numbers
#define STR(s) _str(s)
#define _str(s) #s

#define MAXCMDLEN 6

typedef enum Op {
	NONE,
	PRINT,
	SEARCH,
	INSERT,
	DELETE,
} Op;

typedef struct Command {
	Hashmap* map;
	FILE* LOG_OUTPUT;
	pthread_t tid;
	Op op;
	char name[NAMELEN + 1];
	uint32_t salary;
} Command;

void* thread(void* arg) {
	const Command* command = (Command*)arg;

	switch (command->op) {
	case PRINT: {
		hashmap_print(command->map, 1);
		break;
	}
	case SEARCH: {
		fprintf(command->LOG_OUTPUT, "SEARCH,%s\n", command->name);

		HashRecord record;
		if (hashmap_search(command->map, command->name, &record) < 0) {
			fputs("No Record Found\n", command->LOG_OUTPUT);
		} else {
			record_print(&record);
		}

		break;
	}
	case INSERT: {
		fprintf(
			command->LOG_OUTPUT,
			"INSERT,%s,%" PRIu32 "\n",
			command->name,
			command->salary
		);
		hashmap_insert(command->map, command->name, command->salary);
		break;
	}
	case DELETE: {
		fprintf(command->LOG_OUTPUT, "DELETE,%s\n", command->name);
		hashmap_delete(command->map, command->name);
		break;
	}
	default: {
		abort();
	}
	}

	return NULL;
}

int main() {
	FILE* input = fopen("commands.txt", "r");
	assert(input != NULL);

	LOG_OUTPUT = fopen("output.txt", "w");
	assert(LOG_OUTPUT != NULL);

	unsigned int threads;
	assert(fscanf(input, "threads,%u,0 ", &threads) > 0);

	Command* cmds = malloc(sizeof(*cmds) * threads);

	Hashmap map = hashmap_init();

	Command base_cmd = {.map = &map, .LOG_OUTPUT = LOG_OUTPUT};

	for (unsigned int i = 0; i < threads; i++) {
		Command* cmd = &cmds[i];
		*cmd = base_cmd;

		char op_str[MAXCMDLEN + 1];

		assert(
			fscanf(
				input,
				"%" STR(MAXCMDLEN) "[^,],%" STR(NAMELEN) "[^,],%" PRIu32 " ",
				op_str,
				cmd->name,
				&cmd->salary
			) > 0
		);

		// This is probably one of the most cursed things I have ever written.
		const struct {
			Op op;
			const char* str;
		} table[] = {
			{PRINT, "print"},
			{SEARCH, "search"},
			{INSERT, "insert"},
			{DELETE, "delete"},
		};

		Op op = NONE;
		for (int i = 0; i < (int)(sizeof(table) / sizeof(*table)); i++) {
			if (strcmp(table[i].str, op_str) == 0) {
				op = table[i].op;
			}
		}

		assert(op != NONE);
		cmd->op = op;

		assert(pthread_create(&cmd->tid, NULL, thread, cmd) == 0);
	}

	for (unsigned int i = 0; i < threads; i++) {
		pthread_join(cmds[i].tid, NULL);
	}

	fprintf(
		LOG_OUTPUT,
		"\nNumber of lock acquisitions: %lu\nNumber of lock releases: "
		"%lu\nFinal Table:\n",
		read_acquires + write_acquires,
		read_releases + write_releases
	);
	hashmap_print(&map, 0);
}
