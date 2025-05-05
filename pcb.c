#include <stdio.h>
#include "pcb.h" // Include the header

PCB createPCB(int pid, int memoryStart, int memoryEnd, int priority)
{
    PCB pcb;
    pcb.pid = pid;
    pcb.state = NEW;
    pcb.priority = priority;
    pcb.programCounter = 0;
    pcb.memoryStart = memoryStart;
    pcb.memoryEnd = memoryEnd;
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