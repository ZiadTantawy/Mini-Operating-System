#include <stdio.h>
#include "memory.h"
#include "pcb.h"
#include <string.h>
#include "processLoader.h"
#include "queue.h"
#include "scheduler.h"
#include "gui.h"

int allocateProcessMemory(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    char line[100];
    int startIndex = next_free;
    int instructionIndex = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (next_free >= MEMORY_SIZE)
        {
            printf("Error: Not enough memory for instructions.\n");
            fclose(file);
            return -1;
        }
        line[strcspn(line, "\n")] = 0;
        char instructionName[50];
        sprintf(instructionName, "instruction%d", instructionIndex++);
        writeMemory(next_free++, instructionName, line);
    }
    fclose(file);
    return startIndex;
}
const char* stateToString(ProcessState state) {
    switch (state) {
        case NEW: return "NEW";
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case TERMINATED: return "TERMINATED";
        default: return "UNKNOWN";
    }
}
void reserveVariables()
{
    char varNames[3] = {'a', 'b', 'c'};
    for (int i = 0; i < 3; i++)
    {
        char varName[50];
        sprintf(varName, "%c", varNames[i]); // Use %c for character and varNames array
        writeMemory(next_free++, varName, "");
    }
}

void savePCB(PCB pcb)
{
    char buffer[50];

    sprintf(buffer, "%d", pcb.pid);
    writeMemory(next_free++, "PCB_pid", buffer);

    writeMemory(next_free++, "PCB_state", stateToString(pcb.state));
    sprintf(buffer, "%d", pcb.priority);
    writeMemory(next_free++, "PCB_priority", buffer);

    sprintf(buffer, "%d", pcb.programCounter);
    writeMemory(next_free++, "PCB_pc", buffer);

    sprintf(buffer, "%d", pcb.memoryStart);
    writeMemory(next_free++, "PCB_start", buffer);

    sprintf(buffer, "%d", pcb.memoryEnd);
    writeMemory(next_free++, "PCB_end", buffer);
}

int loadProcess(const char *filename, int pid)
{
    int startIndex = allocateProcessMemory(filename);
    if (startIndex == -1)
        return -1;

    reserveVariables();
    int endIndex = next_free - 1;

    GtkWidget *dialog;
    GtkWidget *content_area;
    GtkWidget *entry;
    GtkWidget *label;
    int activationTime = 0;

    dialog = gtk_dialog_new_with_buttons("Input Time",
                                         GTK_WINDOW(widgets.window),
                                         GTK_DIALOG_MODAL,
                                         "_OK", GTK_RESPONSE_ACCEPT,
                                         NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new("Enter the clock cycle to enqueue the process:");
    entry = gtk_entry_new();

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_container_add(GTK_CONTAINER(content_area), entry);
    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        const char *input = gtk_entry_get_text(GTK_ENTRY(entry));
        activationTime = atoi(input); // You might want more validation
        add_log_message(g_strdup_printf("[LOADER] User entered clock cycle %d for PID %d", activationTime, pid));
    }
    else
    {
        add_log_message("[LOADER] Process scheduling cancelled by user.");
        gtk_widget_destroy(dialog);
        return -1; // user cancelled
    }

    gtk_widget_destroy(dialog);

    PCB pcb = createPCB(pid, startIndex, endIndex, 0, activationTime);
    savePCB(pcb);

    enqueue(&delayedQueue, pcb); // Enqueue the PCB to the ready queue

    return startIndex;
}