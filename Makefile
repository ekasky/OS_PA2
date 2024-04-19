# I used the following link to write this makefile "https://www.gnu.org/software/make/manual/html_node/index.html#SEC_Contents" along with ChatGPT to write this Makefile

CC = gcc
CFLAGS = -Wall -Wextra -I$(INCLUDE_DIR) -pthread -fsanitize=thread
# Directories
SRC_DIR = src
INCLUDE_DIR = includes
BIN_DIR = bin

# Source Files
SRCS = ${wildcard *.c} ${wildcard ${SRC_DIR}/*.c}		# Grabs the main entry point from root and all *.c from src/ dir
OBJS = $(patsubst %.c,$(BIN_DIR)/%.o,$(notdir $(SRCS)))

# Executable
EXE = chash

.PHONY: all clean

all: ${EXE}

# Compile source files to object files
$(BIN_DIR)/%.o: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to executable
$(EXE): $(OBJS) | $(OUT_DIR)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
	
$(OUT_DIR):
	mkdir -p $(OUT_DIR)
	
clean:
	rm -rf $(BIN_DIR)/*.o
	rm -rf *.out output.txt
