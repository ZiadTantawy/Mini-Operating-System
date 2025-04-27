#include "../pcb.c"
#include "scheduler.c"
#include "queue.c"


int getQuantumForLevel(int level) {
    switch (level) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 4;
        case 4: return 8;
        default: return 8;
    }
}

void scheduleMLFQ(PCBQueue* levels[], PCB** runningProcess, int* timeSliceRemaining) {
    if (*runningProcess == NULL) {
        for (int i = 0; i < 4; i++) {
            if (!isEmpty(levels[i])) {
                *runningProcess = dequeue(levels[i]);
                updateState(*runningProcess, RUNNING);
                *timeSliceRemaining = getQuantumForLevel(i + 1); // levels[0]=1, levels[1]=2,...
                break;
            }
        }
    }

    if (*runningProcess != NULL) {
        incrementPC(*runningProcess);
        (*timeSliceRemaining)--;

        if ((*runningProcess)->programCounter >= 5) {
            updateState(*runningProcess, TERMINATED);
            *runningProcess = NULL;
        } else if (*timeSliceRemaining == 0) {
            int nextLevel = (*runningProcess)->priority + 1;
            if (nextLevel > 4) nextLevel = 4;
            setPriority(*runningProcess, nextLevel);
            updateState(*runningProcess, READY);
            enqueue(levels[nextLevel - 1], *runningProcess);
            *runningProcess = NULL;
        }
    }
}
