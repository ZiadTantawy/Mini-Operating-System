#include "pcb.h"
#include "queue.h"
#include "interpreter.h"
#include "memory.h"
#include "scheduler.h"
#include <stdio.h>

void scheduleFCFS_OneStep()
{
    // If no process is currently running, fetch one from the queue
    if (runningPCB.pid == 0 || runningPCB.state == TERMINATED)
    {
        if (!isEmpty(&readyQueue))
        {
            runningPCB = dequeue(&readyQueue);
            updateState(&runningPCB, RUNNING);
        }
        else
        {
            return; // No process to run
        }
    }

    // Execute one instruction
    int pcbMemoryEndIndex = runningPCB.memoryEnd + 1;
    interpret(&runningPCB, pcbMemoryEndIndex);
    // printMemory(clockCycle++);

    // Handle state transitions
    if (runningPCB.state == TERMINATED)
    {
        printf("Process %d finished execution.\n", runningPCB.pid);
        runningPCB.pid = 0; // Reset
    }
    else if (runningPCB.state == BLOCKED)
    {
        printf("Process %d is BLOCKED. Will not re-enqueue.\n", runningPCB.pid);
        runningPCB.pid = 0; // Release current running PCB (mutex will handle requeue)
    }
    else if (runningPCB.state == READY)
    {
        // Stay in runningPCB — still executing
    }
}

// For Start: full FCFS behavior
void scheduleFCFS()
{
    while (!isEmpty(&readyQueue) || runningPCB.pid != 0)
    {
        scheduleFCFS_OneStep();
    }
}
