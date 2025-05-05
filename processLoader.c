#include <stdio.h>
#include "memory.h"
#include "pcb.h"
#include <string.h>
#include "processLoader.h"
#include "queue.h"
#include "scheduler.h"

int allocateProcessMemory(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    char line[100];
    int startIndex = next_free;
    int instructionIndex = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (next_free >= MEMORY_SIZE)
        {
            printf("Error: Not enough memory for instructions.\n");
            fclose(file);
            return -1;
        }
        line[strcspn(line, "\n")] = 0;
        char instructionName[50];
        sprintf(instructionName, "instruction%d", instructionIndex++);
        writeMemory(next_free++, instructionName, line);
    }
    fclose(file);
    return startIndex;
}

void reserveVariables()
{
    for (int i = 0; i < 3; i++)
    {
        char varName[50];
        sprintf(varName, "var_%d", i);
        writeMemory(next_free++, varName, "");
    }
}

void savePCB(PCB pcb)
{
    char buffer[50];

    sprintf(buffer, "%d", pcb.pid);
    writeMemory(next_free++, "PCB_pid", buffer);

    writeMemory(next_free++, "PCB_state", stateToString(pcb.state));
    sprintf(buffer, "%d", pcb.priority);
    writeMemory(next_free++, "PCB_priority", buffer);

    sprintf(buffer, "%d", pcb.programCounter);
    writeMemory(next_free++, "PCB_pc", buffer);

    sprintf(buffer, "%d", pcb.memoryStart);
    writeMemory(next_free++, "PCB_start", buffer);

    sprintf(buffer, "%d", pcb.memoryEnd);
    writeMemory(next_free++, "PCB_end", buffer);
}

int loadProcess(const char *filename, int pid)
{
    int startIndex = allocateProcessMemory(filename);
    if (startIndex == -1)
        return -1;

    reserveVariables();
    int endIndex = next_free - 1;

    PCB pcb = createPCB(pid, startIndex, endIndex, 1);
    savePCB(pcb);
    enqueue(&readyQueue, pcb); // Enqueue the PCB to the ready queue

    return startIndex;
}