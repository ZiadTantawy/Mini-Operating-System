
#ifndef PROCESS_LOADER_H
#define PROCESS_LOADER_H

#include "pcb.h"  // For PCB definition
#include <stddef.h>  // For size_t

// Function declarations
int allocateProcessMemory(const char* filename);
void reserveVariables(void);
void savePCB(PCB pcb);
int loadProcess(const char* filename, int pid);

// Helper function (assuming it exists elsewhere)
const char* stateToString(ProcessState state);

#endif // PROCESS_LOADER_H
