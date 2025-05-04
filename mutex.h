#ifndef MUTEX_H
#define MUTEX_H

#include "pcb.h"
#include "scheduler.h"
#include "queue.h"

// Mutex structure
typedef struct Mutex {
    int isLocked;
    PCB *owner;            // Pointer to owner PCB
    PCBQueue blockedQueue; // Blocked queue for this resource
} Mutex;

// Global mutex variables
extern Mutex userInputMutex;
extern Mutex userOutputMutex;
extern Mutex fileMutex;

// Function declarations
void initMutexes();
void semWait(Mutex *mutex, PCB *process);
void semSignal(Mutex *mutex);

#endif // MUTEX_H