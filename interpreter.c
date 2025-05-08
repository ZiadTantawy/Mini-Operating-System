#include <stdio.h>
#include "memory.h"
#include "pcb.h"
#include "instructionHandlers.h"
#include <string.h>
#include <stdlib.h>
#include "scheduler.h" // For add_log_message
char *fetchInstruction(int pcbEndIndex)
{
    int pc = atoi(readMemory(pcbEndIndex + 3).data);
    int start = atoi(readMemory(pcbEndIndex + 4).data);

    char instructionName[50];
    sprintf(instructionName, "instruction%d", pc);

    for (int i = start; i < MEMORY_SIZE; i++)
    {
        if (strcmp(memory[i].name, instructionName) == 0)
        {
            return memory[i].data;
        }
    }
    printf("Instruction not found!\n");
    return NULL;
}

void executeInstruction(const char *instruction, PCB *pcb)
{
    if (pcb->state != BLOCKED && currentAlgorithm == MLFQ)
    {
        incrementPC(pcb);

        // Save the updated PC to memory
        char buffer[50];
        sprintf(buffer, "%d", pcb->programCounter);
        writeMemory(pcb->memoryEnd + 4, "PCB_pc", buffer);
    }

    if (!instruction)
    {
        updateState(pcb, TERMINATED);
        writeMemory(pcb->memoryEnd + 2, "PCB_state", "TERMINATED");
        return;
    }

    if (strncmp(instruction, "print ", 6) == 0)
    {
        handlePrint(instruction + 6, pcb->memoryEnd + 1);
    }
    else if (strncmp(instruction, "assign ", 7) == 0)
    {
        printf("Assigning...\n");
        handleAssign(instruction + 7, pcb->memoryEnd + 1);
    }
    else if (strncmp(instruction, "semWait ", 8) == 0)
    {
        handleSemWait(instruction + 8, pcb);
    }
    else if (strncmp(instruction, "semSignal ", 10) == 0)
    {
        handleSemSignal(instruction + 10);
    }
    else if (strncmp(instruction, "printFromTo ", 12) == 0)
    {
        handlePrintFromTo(instruction + 12, pcb->memoryEnd + 1);
    }
    else if (strncmp(instruction, "writeFile ", 10) == 0)
    {
        handleWriteFile(instruction + 10);
    }
    else if (strncmp(instruction, "readFile ", 9) == 0)
    {
        handleReadFile(instruction + 9, pcb->memoryEnd + 1);
    }
    else
    {
        printf("Unknown instruction: %s\n", instruction);
    }

    if (pcb->state != BLOCKED && currentAlgorithm != MLFQ)
    {
        incrementPC(pcb);

        // Save the updated PC to memory
        char buffer[50];
        sprintf(buffer, "%d", pcb->programCounter);
        writeMemory(pcb->memoryEnd + 4, "PCB_pc", buffer);
    }
}

void interpret(PCB *pcb, int pcbMemoryEndIndex)
{
    char *instruction = fetchInstruction(pcbMemoryEndIndex);
    executeInstruction(instruction, pcb);
}