// filepath: c:\Users\Manar Badawy\os project\OS_Milestone2\pcb.h
#ifndef PCB_H
#define PCB_H

// Process states
typedef enum {
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// PCB structure
typedef struct PCB {
    int pid;
    ProcessState state; // Use ProcessState type here
    int priority;
    int programCounter;
    int memoryStart;
    int memoryEnd;
    char currentInstruction[100];
    int queueEntryTime;
    int activationTime;
} PCB;

// Declare runningPCB as extern
extern PCB runningPCB;

#endif // PCB_H