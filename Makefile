# Compiler and flags
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS = `pkg-config --libs gtk+-3.0`

# Output binary
TARGET = scheduler_gui

# Source files
SRC = \
    scheduler_gui.c \
    gui_handlers.c \
    interpreter.c \
    instructionHandlers.c \
    processLoader.c \
    memory.c \
    mutex.c \
    main.c \
    scheduler/fcfs.c \
    scheduler/rr.c \
    scheduler/mlfq.c \
    scheduler/scheduler.c \
    scheduler/queue.c

# Object files (replace .c with .o)
OBJ = $(SRC:.c=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean target
clean:
	rm -f $(TARGET) $(OBJ)