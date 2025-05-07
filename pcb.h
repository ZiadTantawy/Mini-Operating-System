// #ifndef PCB_H
// #define PCB_H

// typedef enum
// {
//     NEW,
//     READY,
//     RUNNING,
//     BLOCKED,
//     TERMINATED
// } ProcessState;

// typedef struct PCB
// {
//     int pid;
//     ProcessState state;
//     int priority;
//     int programCounter;
//     int memoryStart;
//     int memoryEnd;
// } PCB;

// // Function declarations
// PCB createPCB(int pid, int memoryStart, int memoryEnd, int priority);
// void updateState(PCB *pcb, ProcessState newState);
// void incrementPC(PCB *pcb);
// const char *stateToString(ProcessState state);
// void printPCB(PCB *pcb);
// void setPriority(PCB *pcb, int newPriority);

// #endif
#ifndef PCB_H
#define PCB_H

typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

typedef struct PCB {
    int pid;
    ProcessState state;
    int priority;
    int programCounter;
    int memoryStart;
    int memoryEnd;
    char currentInstruction[100];
    int queueEntryTime;
} PCB;

// Add external declaration for clockCycle
extern int clockCycle;

// Function declarations
PCB createPCB(int pid, int memoryStart, int memoryEnd, int priority);
void updateState(PCB *pcb, ProcessState newState);
void incrementPC(PCB *pcb);
const char *stateToString(ProcessState state);
void printPCB(PCB *pcb);
void setPriority(PCB *pcb, int newPriority);

#endif