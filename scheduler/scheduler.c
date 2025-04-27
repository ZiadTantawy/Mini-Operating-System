#include "../pcb.c"
#include "queue.c"
#include "../memory.c"

PCBQueue readyQueue;
PCBQueue blockedQueue;
int quantumNumber = 2; // Default quantum
int clockCycle = 0;    // Global clock cycle counter

typedef enum { FCFS, RR, MLFQ } SchedulingAlgorithm;
SchedulingAlgorithm currentAlgorithm = FCFS;

// Initialize queues
void initSchedulers() {
    initQueue(&readyQueue);
    initQueue(&blockedQueue);
    clockCycle = 0;
}

// Set scheduling algorithm
void setSchedulingAlgorithm(SchedulingAlgorithm algo) {
    currentAlgorithm = algo;
}

// Execute one clock cycle
void scheduleOneCycle() {
    switch (currentAlgorithm) {
        case FCFS:
            scheduleFCFS();
            break;
        case RR:
            scheduleRR();
            break;
        case MLFQ:
            scheduleMLFQ();
            break;
    }
}
