#include <stdio.h>
#include "memory.c"
#include "pcb.c"
#include "mutex.c"


void handleAssign(const char* params, int pcbMemoryStartIndex) {
    char varName[50], value[50];
    sscanf(params, "%s %s", varName, value);

    // Search for the variable memory slot
    for (int i = pcbMemoryStartIndex - 6; i < pcbMemoryStartIndex - 3; i++) {
        if (strcmp(memory[i].name, varName) == 0 || strcmp(memory[i].name, "") == 0) {
            if (strcmp(value, "input") == 0) {
                printf("Please enter a value for %s: ", varName);
                scanf("%s", value);
            }
            strcpy(memory[i].name, varName);
            strcpy(memory[i].data, value);
            return;
        }
    }
    printf("Variable %s not found or no space!\n", varName);
}

void handlePrint(const char* varName, int pcbMemoryStartIndex) {
    for (int i = pcbMemoryStartIndex - 6; i < pcbMemoryStartIndex - 3; i++) {
        if (strcmp(memory[i].name, varName) == 0) {
            printf("%s = %s\n", varName, memory[i].data);
            return;
        }
    }
    printf("Variable %s not found!\n", varName);
}

void handleSemWait(const char* resourceName, PCB *pcb, int pcbMemoryStartIndex) {
    if (strcmp(resourceName, "userInput") == 0) {
        semWait(&userInputMutex, pcb);
    } else if (strcmp(resourceName, "userOutput") == 0) {
        semWait(&userOutputMutex, pcb);
    } else if (strcmp(resourceName, "file") == 0) {
        semWait(&fileMutex, pcb);
    } else {
        printf("Unknown resource in semWait: %s\n", resourceName);
    }

    // If the process got BLOCKED, update memory too
    if (pcb->state == BLOCKED) {
        writeMemory(pcbMemoryStartIndex + 1, "PCB_state", "BLOCKED");
    }
}

void handleSemSignal(const char* resourceName, int pcbMemoryStartIndex) {
    if (strcmp(resourceName, "userInput") == 0) {
        semSignal(&userInputMutex);
    } else if (strcmp(resourceName, "userOutput") == 0) {
        semSignal(&userOutputMutex);
    } else if (strcmp(resourceName, "file") == 0) {
        semSignal(&fileMutex);
    } else {
        printf("Unknown resource in semSignal: %s\n", resourceName);
    }
}

void handlePrintFromTo(const char* params, int pcbMemoryStartIndex) {
    char var1[50], var2[50];
    sscanf(params, "%s %s", var1, var2);

    int a = -1, b = -1;

    for (int i = pcbMemoryStartIndex - 6; i < pcbMemoryStartIndex - 3; i++) {
        if (strcmp(memory[i].name, var1) == 0) {
            a = atoi(memory[i].data);
        }
        if (strcmp(memory[i].name, var2) == 0) {
            b = atoi(memory[i].data);
        }
    }

    if (a == -1 || b == -1) {
        printf("Variables for printFromTo not found!\n");
        return;
    }

    for (int i = a; i <= b; i++) {
        printf("%d ", i);
    }
    printf("\n");
}

void handleWriteFile(const char* params, int pcbMemoryStartIndex) {
    char filename[50], data[100];
    sscanf(params, "%s %[^\n]", filename, data);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file %s for writing.\n", filename);
        return;
    }
    fprintf(fp, "%s", data);
    fclose(fp);

    printf("Data written to file %s successfully.\n", filename);
}

void handleReadFile(const char* filename, int pcbMemoryStartIndex) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file %s for reading.\n", filename);
        return;
    }

    char data[100];
    fscanf(fp, "%s", data);
    fclose(fp);

    // Save into a free variable slot
    for (int i = pcbMemoryStartIndex - 6; i < pcbMemoryStartIndex - 3; i++) {
        if (strcmp(memory[i].name, "") == 0) {
            strcpy(memory[i].name, "fileData");
            strcpy(memory[i].data, data);
            printf("File data saved into memory.\n");
            return;
        }
    }
    printf("No free space to save file data.\n");
}