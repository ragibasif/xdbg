#################################################################################
#
# Make commands:
# `make`
# `make all`
# `make clean`

# Object files are compiled with the appropriate flags for each target.
#  Uses `-g` for debugging symbols, `-O2` for optimization, and `-DDEBUG` to enable debug-specific code. Enables all warnings (`-Wall -Wextra -pedantic`) and treats warnings as errors (`-Werror`).
# `-O3` for maximum optimization and `-DNDEBUG` to disable assertions.
# `all`: building the binary
# `clean`: Removes obj and bin files
#
# use tabs instead of spaces
#
#################################################################################

# Compiler
CC = gcc

# compiler flags
#  -Wall -Werror -Wextra -O0 -pedantic -DDEBUG for debug build
#  -Werror: treat warnings like errors
#  -Weverything: display all warnings
DEBUG_CFLAGS = -Wall -Wextra -O0 -g -pedantic -DDEBUG -DXDBG_ENABLE
# -03 -DNDEBUG for release build
RELEASE_CFLAGS = -O3 -DNDEBUG
CFLAGS = $(DEBUG_CFLAGS)
# CFLAGS = $(RELEASE_CFLAGS)

LDFLAGS = # linking flags: -lm -lefence
LIBS =

# Executable names
# name of the final program
TARGET = xdbg

# Source files
SRCS = $(wildcard *.c)

# Object files
# which object files are part of the final program
OBJS = $(SRCS:.c=.o)

default: all

# Link object files to create the executable.
all: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files into object files.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files.
clean:
	$(RM) $(OBJS) $(TARGET) *~ *.bak

# Phony targets
.PHONY: all clean default
