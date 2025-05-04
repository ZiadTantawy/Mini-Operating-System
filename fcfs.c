#include "pcb.h"
#include "queue.h"
#include "interpreter.h"
#include "memory.h"
#include "scheduler.h"
#include <stdio.h>

// Extern global variables
extern PCBQueue readyQueue;
extern int clockCycle;

void scheduleFCFS()
{
    if (isEmpty(&readyQueue)) return;

    
    PCB runningPCB = dequeue(&readyQueue); // Dequeue PCB by value
     // Dequeue PCB by value
    if (runningPCB.pid == 0) return; // Dummy PCB check

    updateState(&runningPCB, RUNNING); // Set state to RUNNING
    int pcbMemoryStartIndex = runningPCB.memoryStart;

    while (runningPCB.state == RUNNING || runningPCB.state == READY)
    {
        interpret(&runningPCB, pcbMemoryStartIndex); // Execute one instruction
        printMemory(clockCycle++); // Print memory after each clock cycle

        if (runningPCB.state == BLOCKED || runningPCB.state == TERMINATED)
            break;
    }

    if (runningPCB.state == TERMINATED)
    {
        printf("Process %d finished execution.\n", runningPCB.pid);
        // No re-enqueue
    }
    else if (runningPCB.state == READY)
    {
        printf("Process %d remains READY. Re-enqueueing.\n", runningPCB.pid);
        enqueue(&readyQueue, runningPCB); // Re-enqueue if still ready
    }
    else if (runningPCB.state == BLOCKED)
    {
        printf("Process %d is BLOCKED. Will not re-enqueue.\n", runningPCB.pid);
        // Handled by mutex
    }
}
