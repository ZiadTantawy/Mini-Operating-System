#include <stdio.h>
#include "scheduler/queue.c"
#include "pcb.c"
#include "scheduler/scheduler.c"  // To access readyQueue, blockedQueue

// Mutex structure
typedef struct Mutex {
    int isLocked;
    PCB* owner;         // Pointer to owner PCB
    PCBQueue blockedQueue; // Blocked queue for this resource
} Mutex;

// Mutex variables
Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

// Extern global queues
extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;

// Initialize all mutexes
void initMutexes() {
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

// semWait function
void semWait(Mutex *mutex, PCB *process)
{
    if (mutex->isLocked == 0) {
        // Mutex is free, acquire it
        mutex->isLocked = 1;
        mutex->owner = process;
        printf("Process %d acquired the mutex.\n", process->pid);
    }
    else {
        // Mutex is already locked, block the process
        printf("Process %d is BLOCKED waiting for mutex.\n", process->pid);
        process->state = BLOCKED;

        enqueue(&mutex->blockedQueue, *process);   // Blocked at resource level
        enqueue(&blockedQueue, *process);           // Also add to global blockedQueue
    }
}

// semSignal function
void semSignal(Mutex *mutex)
{
    if (mutex->isLocked == 0) {
        printf("Warning: semSignal called on unlocked mutex.\n");
        return;
    }

    mutex->isLocked = 0;
    mutex->owner = NULL;

    if (!isEmpty(&mutex->blockedQueue)) {
        // Unblock the next waiting process
        PCB unblockedPCB = dequeue(&mutex->blockedQueue); // Dequeue one PCB

        unblockedPCB.state = READY;
        enqueue(&readyQueue, unblockedPCB); // Move to readyQueue

        printf("Process %d unblocked and moved to READY queue.\n", unblockedPCB.pid);

        // Lock mutex again for the new owner
        mutex->isLocked = 1;
        mutex->owner = &unblockedPCB; // Point to new owner (note: be cautious here if needed)

        // Remove from global blockedQueue manually
        PCBQueue tempQueue;
        initQueue(&tempQueue);

        while (!isEmpty(&blockedQueue)) {
            PCB temp = dequeue(&blockedQueue);
            if (temp.pid != unblockedPCB.pid) {
                enqueue(&tempQueue, temp); // Keep other blocked processes
            }
        }

        // Restore tempQueue back to blockedQueue
        while (!isEmpty(&tempQueue)) {
            enqueue(&blockedQueue, dequeue(&tempQueue));
        }
    }
}
