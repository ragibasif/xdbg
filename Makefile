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
# -Wall -Werror -Wextra -O0 -pedantic -DDEBUG for debug build
# -Weverything: Display all warnings
# -Wall       : Enable all common warnings to catch potential issues
# -Wextra     : Enable extra warnings for stricter code checking
# -Werror     : Treat warnings as errors
# -std=c99    : Enforce C99 standard compliance
# -O0         : No optimization (useful for debugging)
# -O1         : Optimize for size and speed with minimal transformations
# -O2         : Optimize more aggressively while maintaining stability
# -O3         : Enable maximum optimization, including aggressive inlining
# -Os         : Optimize for smallest binary size
# -Og         : Optimize while preserving debugging information
# -march=native: Optimize code for the current machine architecture
# -fPIC       : Generate position-independent code (for shared libraries)
# -fstack-protector-strong : Adds stack protection against buffer overflows
# -g          : Include debugging symbols
# -DNDEBUG    : Disable assertions
# -fno-strict-aliasing : Prevents strict aliasing optimizations


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
# Rule to Compile Object Files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	# '$<' : Represents the source file
	# '$@' : Represents the target object file
	# '-c' : Compile only, do not link


# Clean up build files.
# Clean Target: Remove compiled files and object files
clean:
	$(RM) $(OBJS) $(TARGET) *~ *.bak

run:
	./run.sh

# Prevents Make from treating them as actual files
# Phony targets
.PHONY: all clean default help


help:
	@echo "Makefile for Building XDBG."
	@echo "Usage: make [ all | clean | help | run ]"

