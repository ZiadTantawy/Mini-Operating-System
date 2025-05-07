#include "pcb.h"
#include "queue.h"
#include "interpreter.h"
#include "memory.h"
#include "scheduler.h"
#include <stdio.h>

// Priority queues
PCBQueue priority1Queue = {0};
PCBQueue priority2Queue = {0};
PCBQueue priority3Queue = {0};
PCBQueue priority4Queue = {0};
extern int clockCycle;

// Flag to track if queues have been initialized
static int queuesInitialized = 0;

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

// Helper: demote priority
void demotePriority(PCB *pcb)
{
    if (pcb->priority < 4)
        pcb->priority++;
}

// Initialize MLFQ priority queues
void initializeMlfqQueues()
{
    // Initialize the queues
    initQueue(&priority1Queue);
    initQueue(&priority2Queue);
    initQueue(&priority3Queue);
    initQueue(&priority4Queue);

    // Transfer processes from ready queue to priority queues
    PCBQueue tempQueue;
    initQueue(&tempQueue);

    printf("[MLFQ INIT] Moving processes from ready queue to priority queues\n");

    // First, move all processes to a temporary queue
    while (!isEmpty(&readyQueue))
    {
        PCB pcb = dequeue(&readyQueue);
        enqueue(&tempQueue, pcb);
    }

    // Then, move from temporary queue to priority 1 queue
    while (!isEmpty(&tempQueue))
    {
        PCB pcb = dequeue(&tempQueue);
        pcb.priority = 1; // Start at highest priority
        printf("[MLFQ INIT] PID %d moved to priority queue 1\n", pcb.pid);
        enqueue(&priority1Queue, pcb);
    }

    queuesInitialized = 1;
}

void scheduleMLFQ()
{
    if (!queuesInitialized)
        initializeMlfqQueues();

    while (
        !isEmpty(&priority1Queue) ||
        !isEmpty(&priority2Queue) ||
        !isEmpty(&priority3Queue) ||
        !isEmpty(&priority4Queue) ||
        runningPCB.pid != 0)
    {
        scheduleMLFQ_OneStep();
    }
}

void scheduleMLFQ_OneStep()
{
    if (!queuesInitialized)
        initializeMlfqQueues();

    printf("[MLFQ STEP] Queue status: Q1:%s Q2:%s Q3:%s Q4:%s\n",
           isEmpty(&priority1Queue) ? "empty" : "non-empty",
           isEmpty(&priority2Queue) ? "empty" : "non-empty",
           isEmpty(&priority3Queue) ? "empty" : "non-empty",
           isEmpty(&priority4Queue) ? "empty" : "non-empty");

    if (!isEmpty(&readyQueue))
    {
        while (!isEmpty(&readyQueue))
        {
            PCB pcb = dequeue(&readyQueue);
            pcb.priority = 1;
            enqueue(&priority1Queue, pcb);
            printf("[MLFQ STEP] PID %d moved from ready queue to priority 1\n", pcb.pid);
        }
    }

    if (runningPCB.pid == 0 || runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
    {
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
                printf("[MLFQ STEP] PID %d dequeued from queue %d\n", runningPCB.pid, i + 1);
                break;
            }
        }

        if (runningPCB.pid == 0)
        {
            printf("[MLFQ STEP] No process to run\n");
            return;
        }
    }

    interpret(&runningPCB, runningPCB.memoryEnd + 1);
    mlfqTimeSliceCounter++;

    int timeQuantum = quantumNumber;
    printf("[MLFQ STEP] PID %d executed instruction. Time slice %d/%d\n",
           runningPCB.pid, mlfqTimeSliceCounter, timeQuantum);

    if (runningPCB.state == TERMINATED)
    {
        printf("[MLFQ STEP] PID %d terminated\n", runningPCB.pid);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
    else if (runningPCB.state == BLOCKED)
    {
        printf("[MLFQ STEP] PID %d blocked\n", runningPCB.pid);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
    else if (mlfqTimeSliceCounter >= timeQuantum)
    {
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
        runningPCB.priority = nextLevel + 1;
        enqueue(q, runningPCB);
        printf("[MLFQ STEP] PID %d used full quantum, demoted to queue %d\n",
               runningPCB.pid, nextLevel + 1);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
}