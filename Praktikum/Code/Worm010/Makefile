############################################################
# General purpose makefile
#
# Works for all simple C-projects where
# - binaries are compiled into sub-dir bin
# - binaries are created from a single c-source of the same name
# - we specify the curses library depending on the platform
#
# Note: multiple targets (binaries) in ./bin are supported
#

# Please add targets in ./bin here
TARGETS += $(BIN_DIR)/worm
 
#################################################
# There is no need to edit below this line
#################################################

MACHINE := $(shell uname -m)
ifeq ($(MACHINE), i686)
  CFLAGS = -g -Wall
  LDLIBS = -lncurses
else ifeq ($(MACHINE), armv7l)
  CFLAGS = -g -Wall
  LDLIBS = -lncurses
else ifeq ($(MACHINE), arm64)
  CFLAGS = -g -Wall
  LDLIBS = -lncurses
else ifeq ($(MACHINE), x86_64)
  CFLAGS = -g -Wall
  LDLIBS = -lncurses
endif

#### Fixed variable definitions
CC = gcc
RM_DIR = rm -rf
MKDIR = mkdir
SHELL = /bin/bash
BIN_DIR = bin

# In Makefiles anything in the syntax of     'any:'     or    'any :'     should be seen as a callable function
# Anything after the 'any: $(any2)' will be called as a dependency and be invoked as function call or variable include

# Default target. This is what is ran  when `make` is ran without any arguments like `make clean` or smth.
# This will be run first. First $(BIN_DIR) will be called, creating the folder 'bin'. From '$(BIN_DIR):' below
# The $(TARGETS) will invoke the variable(defined at the beginning of this file)
# When the TARGETS variable at the top is invoked it calls the code in line 54
# This is done now. The execution order mimics the following bash: rm -rf bin && mkdir bin && gcc -g -Wall worm.c -o bin/worm -lncurses
all: $(BIN_DIR) $(TARGETS)

# Fixed build rule for a monolithic C-File (only one source file per binary)
# This matches anything in the format of: $(BIN_DIR)/* and saves the value before the ':' into the $@  and the value after the ':' into the $<
$(BIN_DIR)/% : %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

.PHONY: clean
clean :
	$(RM_DIR) $(BIN_DIR)

