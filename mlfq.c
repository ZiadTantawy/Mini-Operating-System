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
    static PCB runningPCB;
    static int quantumUsed = 0;
    static int hasActiveProcess = 0;

    while (1)
    {
        if (!hasActiveProcess)
        {
            PCBQueue *queue = getNextPriorityQueue();
            if (queue == NULL)
                return;

            runningPCB = dequeue(queue);
            if (runningPCB.pid == 0)
                return;

            quantumUsed = 0;
            hasActiveProcess = 1;
            updateState(&runningPCB, RUNNING);
        }

        interpret(&runningPCB, runningPCB.memoryStart);
        printMemory(clockCycle++);
        quantumUsed++;

        int quantum = getQuantumForPriority(runningPCB.priority);

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
        else if (quantumUsed >= quantum)
        {
            printf("Process %d finished its quantum. Demoting and re-enqueueing.\n", runningPCB.pid);
            demotePriority(&runningPCB);

            if (runningPCB.priority == 1)
                enqueue(&priority1Queue, runningPCB);
            else if (runningPCB.priority == 2)
                enqueue(&priority2Queue, runningPCB);
            else if (runningPCB.priority == 3)
                enqueue(&priority3Queue, runningPCB);
            else
                enqueue(&priority4Queue, runningPCB);

            hasActiveProcess = 0;
        }

        if (!hasActiveProcess && getNextPriorityQueue() == NULL)
            break;
    }
}

// Globals for one-step mode
static PCB mlfq_runningPCB;
static int mlfq_quantumUsed = 0;
static int mlfq_hasActiveProcess = 0;

void scheduleMLFQ_OneStep()
{
    if (!mlfq_hasActiveProcess)
    {
        PCBQueue *queue = getNextPriorityQueue();
        if (queue == NULL)
            return;

        mlfq_runningPCB = dequeue(queue);
        if (mlfq_runningPCB.pid == 0)
            return;

        mlfq_quantumUsed = 0;
        mlfq_hasActiveProcess = 1;
        updateState(&mlfq_runningPCB, RUNNING);
    }

    interpret(&mlfq_runningPCB, mlfq_runningPCB.memoryStart);
    printMemory(clockCycle++);
    mlfq_quantumUsed++;

    int quantum = getQuantumForPriority(mlfq_runningPCB.priority);

    if (mlfq_runningPCB.state == TERMINATED)
    {
        printf("Process %d finished execution.\n", mlfq_runningPCB.pid);
        mlfq_hasActiveProcess = 0;
    }
    else if (mlfq_runningPCB.state == BLOCKED)
    {
        printf("Process %d is BLOCKED. Will not re-enqueue.\n", mlfq_runningPCB.pid);
        mlfq_hasActiveProcess = 0;
    }
    else if (mlfq_quantumUsed >= quantum)
    {
        printf("Process %d quantum ended. Demoting and re-enqueueing.\n", mlfq_runningPCB.pid);
        demotePriority(&mlfq_runningPCB);

        // Enqueue based on new priority
        if (mlfq_runningPCB.priority == 1)
            enqueue(&priority1Queue, mlfq_runningPCB);
        else if (mlfq_runningPCB.priority == 2)
            enqueue(&priority2Queue, mlfq_runningPCB);
        else if (mlfq_runningPCB.priority == 3)
            enqueue(&priority3Queue, mlfq_runningPCB);
        else
            enqueue(&priority4Queue, mlfq_runningPCB);

        mlfq_hasActiveProcess = 0;
    }
}
