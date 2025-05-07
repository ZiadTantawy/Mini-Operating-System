#include <stdio.h>
#include <string.h>      // For strncpy and strcpy
#include "pcb.h"         // Include the header
#include "interpreter.h" // For fetchInstruction
#include "scheduler.h"   // For clockCycle

PCB createPCB(int pid, int memoryStart, int memoryEnd, int priority, int queueEntryTime)
{
    PCB pcb;
    pcb.pid = pid;
    pcb.state = READY;
    pcb.priority = priority;
    pcb.programCounter = 0;
    pcb.memoryStart = memoryStart;
    pcb.memoryEnd = memoryEnd;
    pcb.queueEntryTime = queueEntryTime;

    // Initialize new fields
    pcb.currentInstruction[0] = '\0'; // Empty string
    pcb.queueEntryTime = clockCycle;  // Current clock cycle

    // Update initial instruction
    char *firstInstr = fetchInstruction(memoryEnd);
    if (firstInstr)
    {
        strncpy(pcb.currentInstruction, firstInstr, 99);
        pcb.currentInstruction[99] = '\0';
    }
    else
    {
        strcpy(pcb.currentInstruction, "None");
    }

    return pcb;
}

void updateState(PCB *pcb, ProcessState newState)
{
    if (pcb == NULL)
    {
        printf("Error: NULL PCB pointer passed to updateState.\n");
        return;
    }
    pcb->state = newState;
    pcb->queueEntryTime = clockCycle; // Reset queue time on state change

    // Update current instruction on state change
    char *currentInstr = fetchInstruction(pcb->memoryEnd);
    if (currentInstr)
    {
        strncpy(pcb->currentInstruction, currentInstr, 99);
        pcb->currentInstruction[99] = '\0';
    }
    else
    {
        strcpy(pcb->currentInstruction, "None");
    }
}

void incrementPC(PCB *pcb)
{
    if (pcb == NULL)
    {
        printf("Error: NULL PCB pointer passed to incrementPC.\n");
        return;
    }
    pcb->programCounter++;
}

const char *stateToString(ProcessState state)
{
    switch (state)
    {
    case READY:
        return "READY";
    case RUNNING:
        return "RUNNING";
    case BLOCKED:
        return "BLOCKED";
    case TERMINATED:
        return "TERMINATED";
    default:
        return "UNKNOWN";
    }
}

void printPCB(PCB *pcb)
{
    if (pcb == NULL)
    {
        printf("Error: NULL PCB pointer passed to printPCB.\n");
        return;
    }
    printf("PID: %d\n", pcb->pid);
    printf("State: %s\n", stateToString(pcb->state));
    printf("Priority: %d\n", pcb->priority);
    printf("Program Counter: %d\n", pcb->programCounter);
    printf("Memory Start: %d\n", pcb->memoryStart);
    printf("Memory End: %d\n", pcb->memoryEnd);
    printf("Current Instruction: %s\n", pcb->currentInstruction);
    printf("Time in Queue: %d\n", clockCycle - pcb->queueEntryTime);
}

void setPriority(PCB *pcb, int newPriority)
{
    if (pcb == NULL)
    {
        printf("Error: NULL PCB pointer passed to setPriority.\n");
        return;
    }
    pcb->priority = newPriority;
}