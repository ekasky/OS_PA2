CC := gcc
CFLAGS := -Wall -Wextra -pthread -fsanitize=undefined -g

.PHONY: all
all: chash

.PHONY: run
run: chash
	./chash

.PHONY: clean
clean:
	rm -f chash *.o

chash: chash.o rwlock.o hashdb.o
	$(CC) $(CFLAGS) -o $@ *.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<
