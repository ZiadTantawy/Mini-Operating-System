#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "pcb.h"    // For PCB definition
#include "memory.h" // For memory-related functions

// Instruction fetching and execution functions
char *fetchInstruction(int pcbStartIndex);
void executeInstruction(const char *instruction, PCB *pcb, int pcbMemoryStartIndex);
void interpret(PCB *pcb, int pcbMemoryStartIndex);

// Instruction handler declarations (these would be implemented in instructionHandlers.c)
void handleAssign(const char *params, int pcbMemoryEndIndex);
void handlePrint(const char *varName, int pcbMemoryEndIndex);
void handleSemWait(const char *resourceName, PCB *pcb);
void handleSemSignal(const char *resourceName);
void handlePrintFromTo(const char *params, int pcbMemoryEndIndex);
void handleWriteFile(const char *params);
void handleReadFile(const char *filename, int pcbMemoryEndIndex);
// Helper function for program counter
void incrementPC(PCB *pcb);

#endif // INTERPRETER_H