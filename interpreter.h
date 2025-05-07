#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "pcb.h"

// Function declarations
char* fetchInstruction(int pcbEndIndex);
void executeInstruction(const char* instruction, PCB *pcb);
void interpret(PCB *pcb, int pcbMemoryStartIndex);

#endif