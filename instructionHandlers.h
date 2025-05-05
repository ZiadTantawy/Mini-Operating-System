#ifndef INSTRUCTION_HANDLERS_H
#define INSTRUCTION_HANDLERS_H

#include "pcb.h"

void handleAssign(const char *params, int pcbMemoryEndIndex);
void handlePrint(const char *varName, int pcbMemoryEndIndex);
void handleSemWait(const char *resourceName, PCB *pcb);
void handleSemSignal(const char *resourceName);
void handlePrintFromTo(const char *params, int pcbMemoryEndIndex);
void handleWriteFile(const char *params);
void handleReadFile(const char *filename, int pcbMemoryEndIndex);

#endif