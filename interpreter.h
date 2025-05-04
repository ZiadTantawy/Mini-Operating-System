#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "pcb.h"  // For PCB definition
#include "memory.h" // For memory-related functions

// Instruction fetching and execution functions
char* fetchInstruction(int pcbStartIndex);
void executeInstruction(const char* instruction, PCB *pcb, int pcbMemoryStartIndex);
void interpret(PCB *pcb, int pcbMemoryStartIndex);

// Instruction handler declarations (these would be implemented in instructionHandlers.c)
void handlePrint(const char* args, int pcbMemoryStartIndex);
void handleAssign(const char* args, int pcbMemoryStartIndex);
void handleSemWait(const char* args, PCB *pcb, int pcbMemoryStartIndex);
void handleSemSignal(const char* args, int pcbMemoryStartIndex);
void handlePrintFromTo(const char* args, int pcbMemoryStartIndex);
void handleWriteFile(const char* args, int pcbMemoryStartIndex);
void handleReadFile(const char* args, int pcbMemoryStartIndex);

// Helper function for program counter
void incrementPC(PCB *pcb);

#endif // INTERPRETER_H