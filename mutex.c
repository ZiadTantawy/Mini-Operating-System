#include <stdio.h>
#include "pcb.c"
#include "scheduler/queue.c"

// Define Mutex structure
typedef struct Mutex
{
    int isLocked;
    PCB *owner;
    PCBQueue blockedQueue;
} Mutex;

// Mutex variables
Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

// Extern global queues
extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;

// Initialize all mutexes
void initMutexes()
{
    userInputMutex.isLocked = 0;
    userInputMutex.owner = NULL;
    initQueue(&userInputMutex.blockedQueue);

    userOutputMutex.isLocked = 0;
    userOutputMutex.owner = NULL;
    initQueue(&userOutputMutex.blockedQueue);

    fileMutex.isLocked = 0;
    fileMutex.owner = NULL;
    initQueue(&fileMutex.blockedQueue);
}

// semWait implementation
void semWait(Mutex *mutex, PCB *process)
{
    if (mutex->isLocked == 0)
    {
        // Resource free, process acquires it
        mutex->isLocked = 1;
        mutex->owner = process;
        printf("Process %d acquired the resource.\n", process->pid);
    }
    else
    {
        // Resource busy, process gets blocked
        printf("Process %d is blocked, resource is busy.\n", process->pid);
        process->state = BLOCKED;

        enqueue(&mutex->blockedQueue, *process); // Add to resource-specific blocked queue
        enqueue(&blockedQueue, *process);        // Add to global blocked queue
    }
}

// semSignal implementation
void semSignal(Mutex *mutex)
{
    if (mutex->isLocked == 0)
    {
        printf("Warning: semSignal called on an already free resource.\n");
        return;
    }

    mutex->isLocked = 0;
    mutex->owner = NULL;

    if (!isEmpty(&mutex->blockedQueue))
    {
        // Unblock next process waiting for this resource
        PCB nextProcess = dequeue(&mutex->blockedQueue);
        if (&nextProcess == NULL)
        {
            printf("Error: NULL process dequeued from resource blocked queue.\n");
            return;
        }

        nextProcess->state = READY;
        enqueue(&readyQueue, *nextProcess); // Move to readyQueue

        printf("Process %d is unblocked and moved to Ready Queue.\n", nextProcess->pid);

        // Lock resource again for the newly unblocked process
        mutex->isLocked = 1;
        mutex->owner = nextProcess;

        // Remove from global blockedQueue
        // --- Remove manually by searching and skipping ---
        PCBQueue tempQueue;
        initQueue(&tempQueue);

        while (!isEmpty(&blockedQueue))
        {
            PCB tempPCB = dequeue(&blockedQueue);
            if (tempPCB.pid != nextProcess->pid)
            {
                enqueue(&tempQueue, tempPCB);
            }
        }

        // Restore remaining blocked processes
        while (!isEmpty(&tempQueue))
        {
            enqueue(&blockedQueue, dequeue(&tempQueue));
        }
    }
}
