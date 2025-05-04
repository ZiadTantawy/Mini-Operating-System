#include <stdio.h>
#include "memory.h"
#include "pcb.h"
#include "instructionHandlers.h"
#include <string.h>
#include <stdlib.h>
char* fetchInstruction(int pcbStartIndex) {
    int pc = atoi(readMemory(pcbStartIndex + 8).data);
    int start = atoi(readMemory(pcbStartIndex + 10).data);

    char instructionName[50];
    sprintf(instructionName, "instruction%d", pc);

    for (int i = start; i < MEMORY_SIZE; i++) {
        if (strcmp(memory[i].name, instructionName) == 0) {
            return memory[i].data;
        }
    }
    printf("Instruction not found!\n");
    return NULL;
}

void executeInstruction(const char* instruction, PCB *pcb, int pcbMemoryStartIndex) {
    if (!instruction) {
        updateState(pcb, TERMINATED);
        writeMemory(pcbMemoryStartIndex + 1, "PCB_state", "TERMINATED");
        return;
    }

    if (strncmp(instruction, "print ", 6) == 0) {
        handlePrint(instruction + 6, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "assign ", 7) == 0) {
        handleAssign(instruction + 7, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "semWait ", 8) == 0) {
        handleSemWait(instruction + 8, pcb, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "semSignal ", 10) == 0) {
        handleSemSignal(instruction + 10, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "printFromTo ", 12) == 0) {
        handlePrintFromTo(instruction + 12, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "writeFile ", 10) == 0) {
        handleWriteFile(instruction + 10, pcbMemoryStartIndex);
    } else if (strncmp(instruction, "readFile ", 9) == 0) {
        handleReadFile(instruction + 9, pcbMemoryStartIndex);
    } else {
        printf("Unknown instruction: %s\n", instruction);
    }

    if (pcb->state != BLOCKED) {
        incrementPC(pcb);

        // Save the updated PC to memory
        char buffer[50];
        sprintf(buffer, "%d", pcb->programCounter);
        writeMemory(pcbMemoryStartIndex + 3, "PCB_pc", buffer);
    }
}

void interpret(PCB *pcb, int pcbMemoryStartIndex) {
    char* instruction = fetchInstruction(pcbMemoryStartIndex);
    executeInstruction(instruction, pcb, pcbMemoryStartIndex);
}
