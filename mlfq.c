#include "pcb.h"
#include "queue.h"
#include "interpreter.h"
#include "memory.h"
#include "scheduler.h"
#include <stdio.h>

// Priority queues
// extern PCBQueue priority1Queue;
// extern PCBQueue priority2Queue;
// extern PCBQueue priority3Queue;
// extern PCBQueue priority4Queue;
// Initialize the priority queues (add at file scope)
PCBQueue priority1Queue = {0};
PCBQueue priority2Queue = {0};
PCBQueue priority3Queue = {0};
PCBQueue priority4Queue = {0};
extern int clockCycle;

// Helper: choose next process
PCBQueue *getNextPriorityQueue()
{
    if (!isEmpty(&priority1Queue))
        return &priority1Queue;
    if (!isEmpty(&priority2Queue))
        return &priority2Queue;
    if (!isEmpty(&priority3Queue))
        return &priority3Queue;
    if (!isEmpty(&priority4Queue))
        return &priority4Queue;
    return NULL;
}

// Helper: get quantum per level
int getQuantumForPriority(int priority)
{
    if (priority == 1)
        return 1;
    if (priority == 2)
        return 2;
    if (priority == 3)
        return 4;
    return 8; // priority 4
}

// Helper: demote priority
void demotePriority(PCB *pcb)
{
    if (pcb->priority < 4)
        pcb->priority++;
}

void scheduleMLFQ()
{
    if (runningPCB.pid == 0 || runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
    {
        // Fetch process from highest non-empty queue
        for (int i = 0; i < mlfqQueueCount; i++)
        {
            PCBQueue *q = NULL;
            switch (i)
            {
            case 0:
                q = &priority1Queue;
                break;
            case 1:
                q = &priority2Queue;
                break;
            case 2:
                q = &priority3Queue;
                break;
            case 3:
                q = &priority4Queue;
                break;
            }

            if (!isEmpty(q))
            {
                runningPCB = dequeue(q);
                runningPCB.state = RUNNING;
                currentQueueLevel = i;
                mlfqTimeSliceCounter = 0;
                printf("MLFQ: Running PID %d from priority queue %d\n", runningPCB.pid, i + 1);
                break;
            }
        }

        if (runningPCB.pid == 0)
        {
            printf("MLFQ: No process to schedule\n");
            return;
        }
    }

    int timeQuantum = quantumNumber * (1 << currentQueueLevel);

    while (mlfqTimeSliceCounter < timeQuantum)
    {
        interpret(&runningPCB, runningPCB.memoryEnd + 1);
        mlfqTimeSliceCounter++;

        if (runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
        {
            printf("MLFQ: Process %d terminated or blocked\n", runningPCB.pid);
            runningPCB.pid = 0;
            mlfqTimeSliceCounter = 0;
            return;
        }
    }

    // Time quantum expired: demote and requeue
    if (runningPCB.state == RUNNING)
    {
        int nextLevel = currentQueueLevel < mlfqQueueCount - 1 ? currentQueueLevel + 1 : currentQueueLevel;
        PCBQueue *targetQueue = NULL;
        switch (nextLevel)
        {
        case 0:
            targetQueue = &priority1Queue;
            break;
        case 1:
            targetQueue = &priority2Queue;
            break;
        case 2:
            targetQueue = &priority3Queue;
            break;
        case 3:
            targetQueue = &priority4Queue;
            break;
        }

        runningPCB.state = READY;
        enqueue(targetQueue, runningPCB);
        printf("MLFQ: PID %d demoted to queue %d\n", runningPCB.pid, nextLevel + 1);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
}

void scheduleMLFQ_OneStep()
{
    if (runningPCB.pid == 0 || runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
    {
        // Look for next available process starting from highest priority
        for (int i = 0; i < mlfqQueueCount; i++)
        {
            PCBQueue *q = NULL;
            switch (i)
            {
            case 0:
                q = &priority1Queue;
                break;
            case 1:
                q = &priority2Queue;
                break;
            case 2:
                q = &priority3Queue;
                break;
            case 3:
                q = &priority4Queue;
                break;
            }
            if (!isEmpty(q))
            {
                runningPCB = dequeue(q);
                runningPCB.state = RUNNING;
                currentQueueLevel = i;
                mlfqTimeSliceCounter = 0;
                printf("MLFQ: Running PID %d from queue %d\n", runningPCB.pid, i + 1);
                break;
            }
        }

        if (runningPCB.pid == 0)
        {
            printf("MLFQ: No process to schedule\n");
            return;
        }
    }

    interpret(&runningPCB, runningPCB.memoryEnd + 1);
    mlfqTimeSliceCounter++;

    if (runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
    {
        printf("MLFQ: Process %d terminated or blocked\n", runningPCB.pid);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
    else if (mlfqTimeSliceCounter >= (quantumNumber * (1 << currentQueueLevel)))
    {
        // Time quantum over, demote to next queue
        int nextLevel = currentQueueLevel < mlfqQueueCount - 1 ? currentQueueLevel + 1 : currentQueueLevel;
        PCBQueue *q = NULL;
        switch (nextLevel)
        {
        case 0:
            q = &priority1Queue;
            break;
        case 1:
            q = &priority2Queue;
            break;
        case 2:
            q = &priority3Queue;
            break;
        case 3:
            q = &priority4Queue;
            break;
        }
        runningPCB.state = READY;
        enqueue(q, runningPCB);
        printf("MLFQ: PID %d demoted to queue %d\n", runningPCB.pid, nextLevel + 1);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
}
