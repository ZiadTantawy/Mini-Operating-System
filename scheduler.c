#include "scheduler.h" // Must be first
#include "memory.h"    // For printMemory()
#include <stdio.h>     // For printf()
#include <string.h>    // For strncpy and strcpy
#include "pcb.h"
#include "queue.h"
#include "interpreter.h" // For fetchInstruction
#include "gui.h"         // For add_log_message

SchedulingAlgorithm currentAlgorithm = FCFS;
PCBQueue readyQueue;
PCBQueue blockedQueue;
PCBQueue delayedQueue;
int quantumNumber = 2; // Default quantum
int clockCycle = 0;    // Global clock cycle counter
PCB runningPCB = {0};  // Global currently executing process

int rrTimeSliceCounter = 0;
int mlfqTimeSliceCounter = 0;
int currentQueueLevel = 0;
int mlfqQueueCount = 4;

// typedef enum { FCFS, RR, MLFQ } SchedulingAlgorithm;

// Initialize queues
void initSchedulers()
{
    initQueue(&readyQueue);
    initQueue(&blockedQueue);
    initQueue(&delayedQueue);
    clockCycle = 0;
}

// Set scheduling algorithm
void setSchedulingAlgorithm(SchedulingAlgorithm algo)
{
    currentAlgorithm = algo;
}

void scheduleOneInstruction()
{
    switch (currentAlgorithm)
    {
    case FCFS:
        scheduleFCFS_OneStep();
        break;
    case RR:
        scheduleRR_OneStep(); // To implement next
        break;
    case MLFQ:
        scheduleMLFQ_OneStep(); // To implement next
        break;
    }
}

// Executes one full process or quantum (for Start button)
void scheduleFullProcess()
{
    switch (currentAlgorithm)
    {
    case FCFS:
        scheduleFCFS();
        break;
    case RR:
        scheduleRR(); // Already full quantum
        break;
    case MLFQ:
        scheduleMLFQ(); // Already full quantum
        break;
    }
}

// Replace the static quantum declaration with a getter/setter
void setQuantumNumber(int quantum)
{
    if (currentAlgorithm == RR)
    {
        quantumNumber = quantum;
    }
}

int getQuantumNumber()
{
    return quantumNumber;
}

void updateProcessState(PCB *pcb, ProcessState newState)
{
    pcb->state = newState;
    updateState(pcb, newState); // Update state in memory too
    pcb->queueEntryTime = clockCycle; // Reset queue time on state change

    // Update current instruction
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
void checkDelayedQueue()
{
    // Count the number of PCBs in the delayedQueue
    int count = 0;
    Node *current = delayedQueue.front;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    // Process each PCB in the queue exactly once
    for (int i = 0; i < count; i++)
    {
        PCB dpcb = dequeue(&delayedQueue);

        // Check activation time — NOT queueEntryTime!
        if (dpcb.activationTime <= clockCycle + 1)
        {
            printf("PCB activation Time is %d | Clock Cycle: %d\n", dpcb.activationTime, (clockCycle + 1));
            // Update state to READY
            dpcb.state = READY;
            updateState(&dpcb, READY); // Update state in memory too

            // Update state in memory too
            char stateBuffer[20];
            sprintf(stateBuffer, "%s", stateToString(READY));
            writeMemory(dpcb.memoryEnd + 2, "PCB_state", stateBuffer);

            // Reset queue entry time
            dpcb.queueEntryTime = clockCycle;

            // Enqueue to ready queue
            enqueue(&readyQueue, dpcb);
            add_log_message("[DELAYED] PID %d moved to readyQueue at clock %d", dpcb.pid, clockCycle);
        }
        else
        {
            enqueue(&delayedQueue, dpcb); // Not yet time
        }
    }
    update_gui(&widgets);
}
