#include "../pcb.c"
#include "queue.c"
#include "../interpreter.c"
#include "../memory.c"
#include "scheduler.c"
#include <stdio.h>

// Priority queues
extern PCBQueue priority1Queue;
extern PCBQueue priority2Queue;
extern PCBQueue priority3Queue;
extern PCBQueue priority4Queue;
extern int clockCycle;

// Helper: choose next process
PCBQueue* getNextPriorityQueue() {
    if (!isEmpty(&priority1Queue)) return &priority1Queue;
    if (!isEmpty(&priority2Queue)) return &priority2Queue;
    if (!isEmpty(&priority3Queue)) return &priority3Queue;
    if (!isEmpty(&priority4Queue)) return &priority4Queue;
    return NULL;
}

// Helper: get quantum per level
int getQuantumForPriority(int priority) {
    if (priority == 1) return 1;
    if (priority == 2) return 2;
    if (priority == 3) return 4;
    return 8; // priority 4
}

// Helper: demote priority
void demotePriority(PCB *pcb) {
    if (pcb->priority < 4) pcb->priority++;
}

void scheduleMLFQ()
{
    PCBQueue* queue = getNextPriorityQueue();
    if (queue == NULL) return; // No process to run

    PCB runningPCB = dequeue(queue); // Dequeue PCB by value
    if (runningPCB.pid == 0) return; // Dummy PCB check

    updateState(&runningPCB, RUNNING);
    int pcbMemoryStartIndex = runningPCB.memoryStart;

    int quantum = getQuantumForPriority(runningPCB.priority);
    int quantumUsed = 0;

    while ((runningPCB.state == RUNNING || runningPCB.state == READY) && quantumUsed < quantum)
    {
        interpret(&runningPCB, pcbMemoryStartIndex); // Execute one instruction
        printMemory(clockCycle++); // Print memory after each clock cycle
        quantumUsed++;

        if (runningPCB.state == BLOCKED || runningPCB.state == TERMINATED)
            break;
    }

    if (runningPCB.state == TERMINATED)
    {
        printf("Process %d finished execution.\n", runningPCB.pid);
        // No re-enqueue
    }
    else if (runningPCB.state == READY)
    {
        printf("Process %d finished its quantum. Demoting and re-enqueueing.\n", runningPCB.pid);
        demotePriority(&runningPCB);

        // Enqueue based on new priority
        if (runningPCB.priority == 1) enqueue(&priority1Queue, runningPCB);
        else if (runningPCB.priority == 2) enqueue(&priority2Queue, runningPCB);
        else if (runningPCB.priority == 3) enqueue(&priority3Queue, runningPCB);
        else enqueue(&priority4Queue, runningPCB);
    }
    else if (runningPCB.state == BLOCKED)
    {
        printf("Process %d is BLOCKED. Will not re-enqueue.\n", runningPCB.pid);
        // Blocked processes are managed by mutex logic
    }
}
