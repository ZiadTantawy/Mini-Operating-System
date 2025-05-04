
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "pcb.h"

#define MEMORY_SIZE 60
#define MAX_PROCESSES 10

//typedef enum { NEW, READY, RUNNING, BLOCKED, TERMINATED } ProcessState;

// Memory word structure
typedef struct {
    char name[50];  // Key (e.g., "PCB_1_pid", "var_x")
    char data[50];  // Value (e.g., "123", "READY", "42")
} MemoryWord;


// Global memory array
MemoryWord memory[MEMORY_SIZE];
int next_free = 0;


// Initialize all memory words to empty
void initializeMemory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        strcpy(memory[i].name, "");
        strcpy(memory[i].data, "");
    }
    next_free = 0;
}

// Allocate contiguous memory block
int allocateMemory(int size) {
    if (next_free + size > MEMORY_SIZE) {
        printf("Memory allocation failed: Not enough space\n");
        return -1;
    }
    int start = next_free;
    next_free += size;
    return start;
}

// Write data into the memory
void writeMemory(int index, const char* name, const char* value) {
    if (index < 0 || index >= MEMORY_SIZE) {
        printf("Error: Memory index out of bounds!\n");
        return;
    }
    strcpy(memory[index].name, name);
    strcpy(memory[index].data, value);
}

// Read data from the memory 
MemoryWord readMemory(int index) {
    MemoryWord emptyWord = {"", ""};
    if (index < 0 || index >= MEMORY_SIZE) {
        printf("Error: Memory index out of bounds!\n");
        return emptyWord;
    }
    return memory[index];
}

// Deallocate memory block
void deallocateMemory(int start, int end) {
    if (start < 0 || end >= MEMORY_SIZE || start > end) {
        printf("Invalid memory range for deallocation\n");
        return;
    }
    for (int i = start; i <= end; i++) {
        strcpy(memory[i].name, "");
        strcpy(memory[i].data, "");
    }
}

void printMemory(int cycle) {
    printf("\n=== Memory State (Cycle %d) ===\n", cycle);
    printf("+------+----------------------+----------------------+\n");
    printf("| Addr | Name                 | Value                |\n");
    printf("+------+----------------------+----------------------+\n");
    
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (strlen(memory[i].name) > 0 || strlen(memory[i].data) > 0) {
            // Truncate long values for neat display
            char name_display[20];
            char data_display[20];
            strncpy(name_display, memory[i].name, 19);
            strncpy(data_display, memory[i].data, 19);
            name_display[19] = '\0';
            data_display[19] = '\0';
            
            printf("| %4d | %-20s | %-20s |\n", 
                   i, name_display, data_display);
        }
    }
    printf("+------+----------------------+----------------------+\n");
    printf("Next free: %d\n\n", next_free);
}

// Print memory contents
// void printMemory() {
//     printf("\nMemory Contents:\n");
//     printf("+-----+----------------------+----------------------+\n");
//     printf("| Addr| Name                 | Data                 |\n");
//     printf("+-----+----------------------+----------------------+\n");
    
//     for (int i = 0; i < MEMORY_SIZE; i++) {
//         if (strlen(memory[i].name)) {
//             printf("| %3d | %-20s | %-20s |\n", i, memory[i].name, memory[i].data);
//         } else {
//             printf("| %3d | %-20s | %-20s |\n", i, "(free)", "");
//         }
//     }
//     printf("+-----+----------------------+----------------------+\n");
// }


