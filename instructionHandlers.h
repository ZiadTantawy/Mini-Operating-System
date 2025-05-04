#ifndef INSTRUCTION_HANDLERS_H
#define INSTRUCTION_HANDLERS_H

#include "pcb.h"

void handleAssign(const char* params, int pcbMemoryStartIndex);
void handlePrint(const char* varName, int pcbMemoryStartIndex);
void handleSemWait(const char* resourceName, PCB *pcb, int pcbMemoryStartIndex);
void handleSemSignal(const char* resourceName, int pcbMemoryStartIndex);
void handlePrintFromTo(const char* params, int pcbMemoryStartIndex);
void handleWriteFile(const char* params, int pcbMemoryStartIndex);
void handleReadFile(const char* filename, int pcbMemoryStartIndex);

#endif