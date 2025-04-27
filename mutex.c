#include <stdio.h>
#include <pcb.c>

typedef struct mutex{
    int isLocked;
    PCB* owner;

};