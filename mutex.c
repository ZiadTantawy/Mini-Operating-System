#include <stdio.h>
#include <pcb.c>
#include <queue.c>

typedef struct mutex{
    int isLocked;
    PCB* owner;
    PCBQueue blockedQueue;
} mutex;

mutex userInputMutex;
mutex userOutputMutex;
mutex fileMutex;

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