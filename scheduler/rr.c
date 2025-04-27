#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"


extern int roundRobinQuantum; // Assume it is set in your main/scheduler setup

void scheduleRR(PCBQueue* readyQueue, PCB** runningProcess, int* timeSliceRemaining) {
    if (*runningProcess == NULL) {
        *runningProcess = dequeue(readyQueue);
        if (*runningProcess != NULL) {
            updateState(*runningProcess, RUNNING);
            *timeSliceRemaining = roundRobinQuantum;
        }
    }

    if (*runningProcess != NULL) {
        incrementPC(*runningProcess);
        (*timeSliceRemaining)--;

        if ((*runningProcess)->programCounter >= 5) {
            updateState(*runningProcess, TERMINATED);
            *runningProcess = NULL;
        } else if (*timeSliceRemaining == 0) {
            updateState(*runningProcess, READY);
            enqueue(readyQueue, *runningProcess);
            *runningProcess = NULL;
        }
    }
}
