CC = gcc
CFLAGS = -I. -pthread -g

# Source files
SOURCES = chash.c hashdb.c rwlocks.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = chash

# Default target
all: $(EXECUTABLE)

# Link the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(CFLAGS)

# Compile source files into objects
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean the build
clean:
	rm -f *.o $(EXECUTABLE)

# Run the program
run: $(EXECUTABLE)
	./$(EXECUTABLE)

.PHONY: all clean run
