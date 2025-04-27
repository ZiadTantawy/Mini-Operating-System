#include <stdio.h>
#include <pcb.c>
#include <queue.c>

typedef struct Mutex{
    int isLocked;
    PCB* owner;
    PCBQueue blockedQueue;
} Mutex;

Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

void initMutexes(){
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

void semWait(Mutex *mutex, PCB *process) {
    if (mutex->isLocked == 0) {
        mutex->isLocked = 1;
        mutex->owner = process;
        printf("Process %d acquired the resource.\n", process->pid);
    } else {
        printf("Process %d is blocked, resource is busy.\n", process->pid);
        process->state = BLOCKED;
        enqueue(&mutex->blockedQueue, *process); // Insert process into blocked queue
        // TODO: Also move to general blocked queue later
    }
}

void semSignal(Mutex *mutex) {
    if (mutex->isLocked == 0) {
        printf("Warning: semSignal called on an already free resource.\n");
        return;
    }

    mutex->isLocked = 0;
    mutex->owner = NULL;

    if (!isEmpty(&mutex->blockedQueue)) {
        PCB* nextProcess = dequeue(&mutex->blockedQueue); // Get next highest-priority process
        nextProcess->state = READY;
        printf("Process %d is unblocked and acquired the resource.\n", nextProcess->pid);
        // Re-lock resource to this newly unblocked process
        mutex->isLocked = 1;
        mutex->owner = &nextProcess;

        // TODO: Move nextProcess back to Ready Queue
    }
}