#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "pcb.h"
#include "queue.h"  // Changed from queue.c to queue.h

// External declarations of global queues
extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;
extern int quantumNumber;
extern int clockCycle;
extern PCBQueue priority1Queue;
extern PCBQueue priority2Queue;
extern PCBQueue priority3Queue;
extern PCBQueue priority4Queue;
// Scheduling algorithms enum
typedef enum { FCFS, RR, MLFQ } SchedulingAlgorithm;
extern SchedulingAlgorithm currentAlgorithm;

// Function declarations
void initSchedulers(void);
void setSchedulingAlgorithm(SchedulingAlgorithm algo);
void scheduleOneCycle(void);
void scheduleFCFS(void);  // Assuming these exist based on your switch statement
void scheduleRR(void);
void scheduleMLFQ(void);

#endif // SCHEDULER_H