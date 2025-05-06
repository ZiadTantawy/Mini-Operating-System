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
    
    printf("MLFQ: Moving processes from ready queue to priority queues\n");
    
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
        printf("MLFQ: Moving process PID %d to priority queue 1\n", pcb.pid);
        enqueue(&priority1Queue, pcb);
    }
    
    queuesInitialized = 1;
}

void scheduleMLFQ()
{
    // Initialize queues if not already done
    if (!queuesInitialized)
    {
        initializeMlfqQueues();
    }
    
    // Debug output - show queue status
    printf("MLFQ: Queue status - Q1:%s, Q2:%s, Q3:%s, Q4:%s\n",
           isEmpty(&priority1Queue) ? "empty" : "has processes",
           isEmpty(&priority2Queue) ? "empty" : "has processes",
           isEmpty(&priority3Queue) ? "empty" : "has processes",
           isEmpty(&priority4Queue) ? "empty" : "has processes");

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
    printf("MLFQ: Time quantum for level %d is %d\n", currentQueueLevel + 1, timeQuantum);

    while (mlfqTimeSliceCounter < timeQuantum)
    {
        interpret(&runningPCB, runningPCB.memoryEnd + 1);
        mlfqTimeSliceCounter++;
        printf("MLFQ: Executed instruction for PID %d, time slice %d/%d\n", 
               runningPCB.pid, mlfqTimeSliceCounter, timeQuantum);

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
        runningPCB.priority = nextLevel + 1; // Update priority to match queue level
        enqueue(targetQueue, runningPCB);
        printf("MLFQ: PID %d used full quantum, demoted to queue %d\n", runningPCB.pid, nextLevel + 1);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
}

void scheduleMLFQ_OneStep()
{
    // Initialize queues if not already done
    if (!queuesInitialized)
    {
        initializeMlfqQueues();
    }
    
    // Debug output - show queue status
    printf("MLFQ: Queue status - Q1:%s, Q2:%s, Q3:%s, Q4:%s\n",
           isEmpty(&priority1Queue) ? "empty" : "has processes",
           isEmpty(&priority2Queue) ? "empty" : "has processes",
           isEmpty(&priority3Queue) ? "empty" : "has processes",
           isEmpty(&priority4Queue) ? "empty" : "has processes");
    
    // Handle newly loaded processes that went into the ready queue
    if (!isEmpty(&readyQueue))
    {
        printf("MLFQ: Found processes in ready queue, moving to priority 1\n");
        while (!isEmpty(&readyQueue))
        {
            PCB pcb = dequeue(&readyQueue);
            pcb.priority = 1;
            enqueue(&priority1Queue, pcb);
            printf("MLFQ: Moved PID %d to priority queue 1\n", pcb.pid);
        }
    }

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
    
    int timeQuantum = quantumNumber * (1 << currentQueueLevel);
    printf("MLFQ: Executed instruction for PID %d, time slice %d/%d\n", 
           runningPCB.pid, mlfqTimeSliceCounter, timeQuantum);

    if (runningPCB.state == TERMINATED || runningPCB.state == BLOCKED)
    {
        printf("MLFQ: Process %d terminated or blocked\n", runningPCB.pid);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
    else if (mlfqTimeSliceCounter >= timeQuantum)
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
        runningPCB.priority = nextLevel + 1; // Update priority to match queue level
        enqueue(q, runningPCB);
        printf("MLFQ: PID %d used full quantum, demoted to queue %d\n", runningPCB.pid, nextLevel + 1);
        runningPCB.pid = 0;
        mlfqTimeSliceCounter = 0;
    }
}
