#include "pcb.h"
#include "queue.h"
#include "interpreter.h"
#include "memory.h"
#include "scheduler.h"
#include <stdio.h>

// Extern global variables
extern PCBQueue readyQueue;
extern int clockCycle;
extern int quantumNumber; // Number of instructions allowed per process in a turn

void scheduleRR()
{
    static PCB runningPCB;
    static int quantumUsed = 0;
    static int hasActiveProcess = 0;

    while (1)
    {
        if (!hasActiveProcess)
        {
            if (isEmpty(&readyQueue))
                return;

            runningPCB = dequeue(&readyQueue);
            if (runningPCB.pid == 0)
                return;

            quantumUsed = 0;
            hasActiveProcess = 1;
            updateState(&runningPCB, RUNNING);
        }

        interpret(&runningPCB, runningPCB.memoryStart);
        printMemory(clockCycle++);
        quantumUsed++;

        if (runningPCB.state == TERMINATED)
        {
            printf("Process %d finished execution.\n", runningPCB.pid);
            hasActiveProcess = 0;
        }
        else if (runningPCB.state == BLOCKED)
        {
            printf("Process %d is BLOCKED. Will not re-enqueue.\n", runningPCB.pid);
            hasActiveProcess = 0;
        }
        else if (quantumUsed >= quantumNumber)
        {
            printf("Process %d used quantum. Re-enqueueing.\n", runningPCB.pid);
            enqueue(&readyQueue, runningPCB);
            hasActiveProcess = 0;
        }

        if (!hasActiveProcess && isEmpty(&readyQueue))
            break;
    }
}
// Global static for one-step logic
static PCB rr_runningPCB;
static int rr_quantumUsed = 0;
static int rr_hasActiveProcess = 0;

void scheduleRR_OneStep()
{
    if (!rr_hasActiveProcess)
    {
        if (isEmpty(&readyQueue))
            return;

        rr_runningPCB = dequeue(&readyQueue);
        if (rr_runningPCB.pid == 0)
            return;

        rr_quantumUsed = 0;
        rr_hasActiveProcess = 1;
        updateState(&rr_runningPCB, RUNNING);
    }

    interpret(&rr_runningPCB, rr_runningPCB.memoryStart);
    printMemory(clockCycle++);
    rr_quantumUsed++;

    if (rr_runningPCB.state == TERMINATED)
    {
        printf("Process %d finished execution.\n", rr_runningPCB.pid);
        rr_hasActiveProcess = 0;
    }
    else if (rr_runningPCB.state == BLOCKED)
    {
        printf("Process %d is BLOCKED. Will not re-enqueue.\n", rr_runningPCB.pid);
        rr_hasActiveProcess = 0;
    }
    else if (rr_quantumUsed >= quantumNumber)
    {
        printf("Process %d quantum ended. Re-enqueueing.\n", rr_runningPCB.pid);
        enqueue(&readyQueue, rr_runningPCB);
        rr_hasActiveProcess = 0;
    }
}
