#ifndef MEMORY_H
#define MEMORY_H

#include "pcb.h"

#define MEMORY_SIZE 60

typedef struct {
    char name[50];
    char data[50];
} MemoryWord;

extern MemoryWord memory[MEMORY_SIZE];
extern int next_free;

void initializeMemory();
int allocateMemory(int size);
void writeMemory(int index, const char* name, const char* value);
MemoryWord readMemory(int index);
void deallocateMemory(int start, int end);
//void printMemory();
void printMemory(int cycle);  
#endif