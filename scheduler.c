#include "scheduler.h" // Must be first
#include "memory.h"    // For printMemory()
#include <stdio.h>     // For printf()
#include "pcb.h"
#include "queue.h"

SchedulingAlgorithm currentAlgorithm = FCFS;
PCBQueue readyQueue;
PCBQueue blockedQueue;
int quantumNumber = 2; // Default quantum
int clockCycle = 0;    // Global clock cycle counter

// typedef enum { FCFS, RR, MLFQ } SchedulingAlgorithm;

// Initialize queues
void initSchedulers()
{
    initQueue(&readyQueue);
    initQueue(&blockedQueue);
    clockCycle = 0;
}

// Set scheduling algorithm
void setSchedulingAlgorithm(SchedulingAlgorithm algo)
{
    currentAlgorithm = algo;
}

// // Execute one clock cycle
// void scheduleOneCycle()
// {
//     switch (currentAlgorithm)
//     {
//     case FCFS:
//         scheduleFCFS();
//         break;
//     case RR:
//         scheduleRR();
//         break;
//     case MLFQ:
//         scheduleMLFQ();
//         break;
//     }
// }

// Executes one instruction (for Step button)
void scheduleOneInstruction()
{
    switch (currentAlgorithm)
    {
    case FCFS:
        scheduleFCFS_OneStep();
        break;
    case RR:
        scheduleRR_OneStep(); // To implement next
        break;
    case MLFQ:
        scheduleMLFQ_OneStep(); // To implement next
        break;
    }
}

// Executes one full process or quantum (for Start button)
void scheduleFullProcess()
{
    switch (currentAlgorithm)
    {
    case FCFS:
        scheduleFCFS();
        break;
    case RR:
        scheduleRR(); // Already full quantum
        break;
    case MLFQ:
        scheduleMLFQ(); // Already full quantum
        break;
    }
}
