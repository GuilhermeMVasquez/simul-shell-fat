# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Executable name
TARGET = main

# Source files
SRC = main.c shell.c colors.c commands.c formattedOutput.c fat.c

# Default target
all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean up the executable
clean:
	rm -f $(TARGET)