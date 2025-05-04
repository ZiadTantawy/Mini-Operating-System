
# # Compiler and flags
# CC = gcc
# CFLAGS = -Wall -Wextra `pkg-config --cflags gtk+-3.0`
# LDFLAGS = `pkg-config --libs gtk+-3.0`

# # Output binary
# TARGET = scheduler_gui

# # Source files
# SRC = \
#     main.c \
#     gui.c \
#     scheduler_gui.c \
#     gui_handlers.c \
#     interpreter.c \
#     instructionHandlers.c \
#     processLoader.c \
#     memory.c \
#     mutex.c \
#     pcb.c \
#     fcfs.c \
#     rr.c \
#     mlfq.c \
#     scheduler.c \
#     queue.c

# # Header files
# HEADERS = \
#     gui.h \
#     gui_handlers.h \
#     interpreter.h \
#     instructionHandlers.h \
#     memory.h \
#     mutex.h \
#     pcb.h \
#     scheduler.h \
#     queue.h\
#     processLoader.h
# # Object files
# OBJ = $(SRC:.c=.o)

# # Default target
# all: $(TARGET)

# $(TARGET): $(OBJ)
# 	$(CC) -o $@ $^ $(LDFLAGS)

# %.o: %.c $(HEADERS)
# 	$(CC) -c $< -o $@ $(CFLAGS)

# # Clean target
# clean:
# 	rm -f $(TARGET) $(OBJ)

# .PHONY: all clean

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I. `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Output binary
TARGET = scheduler_gui

# Source files
SRC = \
    main.c \
    gui.c \
    scheduler_gui.c \
    gui_handlers.c \
    interpreter.c \
    instructionHandlers.c \
    processLoader.c \
    memory.c \
    mutex.c \
    pcb.c \
    fcfs.c \
    rr.c \
    mlfq.c \
    scheduler.c \
    queue.c

# Header files (for dependency tracking)
HEADERS = \
    gui.h \
    gui_handlers.h \
    interpreter.h \
    instructionHandlers.h \
    memory.h \
    mutex.h \
    pcb.h \
    processLoader.h \
    scheduler.h \
    queue.h

# Object files
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

# Pattern rule with header dependencies
%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean target
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean