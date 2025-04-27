#include "../pcb.c"
#include "queue.c"

// SchedulingAlgorithm currentAlgorithm = FCFS;
// int roundRobinQuantum = 2; // Default

// void schedule(Queue* readyQueue, Queue* levels[], PCB** runningProcess, int* timeSliceRemaining) {
//     switch (currentAlgorithm) {
//         case FCFS:
//             scheduleFCFS(readyQueue, runningProcess);
//             break;
//         case RR:
//             scheduleRR(readyQueue, runningProcess, timeSliceRemaining);
//             break;
//         case MLFQ:
//             scheduleMLFQ(levels, runningProcess, timeSliceRemaining);
//             break;
//     }
// }

extern PCBQueue readyQueue;
extern PCBQueue blockedQueue;
extern int quantumNumber;

