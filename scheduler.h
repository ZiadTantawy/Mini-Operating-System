#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include "queue.h" // Changed from queue.c to queue.h

// External declarations of global queues
extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;
extern PCBQueue delayedQueue;
extern int quantumNumber;
extern int clockCycle;
extern PCBQueue priority1Queue;
extern PCBQueue priority2Queue;
extern PCBQueue priority3Queue;
extern PCBQueue priority4Queue;
extern PCB runningPCB;
extern int rrTimeSliceCounter;
extern int mlfqTimeSliceCounter;
extern int currentQueueLevel;
extern int mlfqQueueCount;
// Scheduling algorithms enum
typedef enum
{
    FCFS,
    RR,
    MLFQ
} SchedulingAlgorithm;
extern SchedulingAlgorithm currentAlgorithm;

// Function declarations
void initSchedulers(void);
void setSchedulingAlgorithm(SchedulingAlgorithm algo);
void scheduleOneCycle(void);
void scheduleFCFS(void); // Assuming these exist based on your switch statement
void scheduleRR(void);
void scheduleMLFQ(void);
void scheduleOneInstruction();
void scheduleFullProcess();
void scheduleFCFS_OneStep();
void scheduleRR_OneStep();
void scheduleMLFQ_OneStep();
void setQuantumNumber(int quantum);
void checkDelayedQueue();
int getQuantumNumber();

#endif // SCHEDULER_H