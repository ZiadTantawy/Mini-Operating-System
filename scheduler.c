#include "scheduler.h" // Must be first
#include "memory.h"    // For printMemory()
#include <stdio.h>     // For printf()
#include <string.h>    // For strncpy and strcpy
#include "pcb.h"
#include "queue.h"
#include "interpreter.h" // For fetchInstruction
#include "gui.h"         // For add_log_message

// Extern declarations for mutexes
extern Mutex userInputMutex;
extern Mutex userOutputMutex;
extern Mutex fileMutex;

// Define runningPCB
PCB runningPCB = {0}; // Initialize runningPCB

SchedulingAlgorithm currentAlgorithm = FCFS;
PCBQueue readyQueue;
PCBQueue blockedQueue;
PCBQueue delayedQueue;
int quantumNumber = 2; // Default quantum
int clockCycle = 0;    // Global clock cycle counter

int rrTimeSliceCounter = 0;
int mlfqTimeSliceCounter = 0;
int currentQueueLevel = 0;
int mlfqQueueCount = 4;

// Initialize queues
void initSchedulers()
{
    initQueue(&readyQueue);
    initQueue(&blockedQueue);
    initQueue(&delayedQueue);
    clockCycle = 0;
}

// Set scheduling algorithm
void setSchedulingAlgorithm(SchedulingAlgorithm algo)
{
    currentAlgorithm = algo;
}

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

    // Add mutex cleanup logic here if the process finishes execution
    if (runningPCB.state == TERMINATED)
    {
        // RELEASE MUTEXES WHEN PROCESS TERMINATES
        if (userInputMutex.owner != NULL && userInputMutex.owner->pid == runningPCB.pid) {
            semSignal(&userInputMutex);
        }
        if (userOutputMutex.owner != NULL && userOutputMutex.owner->pid == runningPCB.pid) {
            semSignal(&userOutputMutex);
        }
        if (fileMutex.owner != NULL && fileMutex.owner->pid == runningPCB.pid) {
            semSignal(&fileMutex);
        }

        // Reset runningPCB after releasing mutexes
        runningPCB = (PCB){
            .pid = 0,
            .state = NEW,
            .priority = 0,
            .programCounter = 0,
            .memoryStart = 0,
            .memoryEnd = 0
        };
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

// Replace the static quantum declaration with a getter/setter
void setQuantumNumber(int quantum)
{
    if (currentAlgorithm == RR)
    {
        quantumNumber = quantum;
    }
}

int getQuantumNumber()
{
    return quantumNumber;
}

void updateProcessState(PCB *pcb, ProcessState newState)
{
    pcb->state = newState;
    pcb->queueEntryTime = clockCycle; // Reset queue time on state change

    // Update current instruction
    char *currentInstr = fetchInstruction(pcb->memoryEnd);
    if (currentInstr)
    {
        strncpy(pcb->currentInstruction, currentInstr, 99);
        pcb->currentInstruction[99] = '\0';
    }
    else
    {
        strcpy(pcb->currentInstruction, "None");
    }
}

void checkDelayedQueue()
{
    // Count the number of PCBs in the delayedQueue
    int count = 0;
    Node *current = delayedQueue.front;
    while (current != NULL)
    {
        count++;
        current = current->next;
    }
    // Process each PCB in the queue exactly once
    for (int i = 0; i < count; i++)
    {
        PCB dpcb = dequeue(&delayedQueue);

        // Check activation time — NOT queueEntryTime!
        if (dpcb.activationTime == clockCycle + 1)
        {
            // Update state to READY
            dpcb.state = READY;

            // Update state in memory too
            char stateBuffer[20];
            sprintf(stateBuffer, "%s", stateToString(READY));
            writeMemory(dpcb.memoryEnd + 2, "PCB_state", stateBuffer);

            // Reset queue entry time
            dpcb.queueEntryTime = clockCycle;

            // Enqueue to ready queue
            enqueue(&readyQueue, dpcb);
            add_log_message("[DELAYED] PID %d moved to readyQueue at clock %d", dpcb.pid, clockCycle);
        }
        else
        {
            enqueue(&delayedQueue, dpcb); // Not yet time
        }
    }
    update_gui(&widgets);
}
