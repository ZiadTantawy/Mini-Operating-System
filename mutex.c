#include <stdio.h>
#include "scheduler.h"
#include "queue.h"
#include "pcb.h"

// Mutex structure
typedef struct Mutex
{
    int isLocked;
    PCB *owner;            // Pointer to owner PCB
    PCBQueue blockedQueue; // Blocked queue for this resource
} Mutex;

// Mutex instances
Mutex userInputMutex;
Mutex userOutputMutex;
Mutex fileMutex;

// External global queues
extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;
extern PCB runningPCB;

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

// semWait logic
void semWait(Mutex *mutex, PCB *process)
{
    if (mutex->isLocked == 0)
    {
        mutex->isLocked = 1;
        mutex->owner = process;
        printf("Process %d acquired the mutex.\n", process->pid);
    }
    else
    {
        printf("Process %d is BLOCKED waiting for mutex.\n", process->pid);
        process->state = BLOCKED;
        updateState(process, BLOCKED);
        enqueue(&mutex->blockedQueue, *process);
        enqueue(&blockedQueue, *process);
        runningPCB.pid = 0; // Free the CPU
    }
}

void semSignal(Mutex *mutex)
{
    if (mutex->isLocked == 0)
    {
        printf("Warning: semSignal called on unlocked mutex.\n");
        return;
    }

    mutex->isLocked = 0;
    mutex->owner = NULL;

    if (!isEmpty(&mutex->blockedQueue))
    {

        PCB unblockedPCB = dequeue(&mutex->blockedQueue);

        PCBQueue tempQueue;
        initQueue(&tempQueue);

        while (!isEmpty(&blockedQueue))
        {
            PCB temp = dequeue(&blockedQueue);
            if (temp.pid != unblockedPCB.pid)
                enqueue(&tempQueue, temp);
        }

        while (!isEmpty(&tempQueue))
            enqueue(&blockedQueue, dequeue(&tempQueue));

        unblockedPCB.state = READY;
        updateState(&unblockedPCB, READY);


        if (runningPCB.pid == 0)
        {
            runningPCB = unblockedPCB;
            runningPCB.state = RUNNING;
            updateState(&runningPCB, RUNNING);
            printf("Process %d immediately acquired mutex and is now RUNNING.\n", runningPCB.pid);
        }
        else
        {
            enqueue(&readyQueue, unblockedPCB);
            printf("Process %d was unblocked and moved to READY queue.\n", unblockedPCB.pid);
        }
    }
}
