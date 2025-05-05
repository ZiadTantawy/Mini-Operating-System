#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "mutex.h"
#include "queue.h"
// Global GTK widgets
typedef struct {
    GtkWidget *window;
    GtkWidget *notebook;
    
    // Dashboard widgets
    GtkWidget *clock_cycle_label;
    GtkWidget *algorithm_label;
    GtkWidget *process_list;
    GtkWidget *ready_queue;
    GtkWidget *blocked_queue;
    GtkWidget *running_process;
    
    // Control panel widgets
    GtkWidget *algorithm_combo;
    GtkWidget *quantum_spin;
    GtkWidget *start_button;
    GtkWidget *stop_button;
    GtkWidget *reset_button;
    GtkWidget *step_button;
    
    // Resource panel widgets
    GtkWidget *mutex_status[3];
    GtkWidget *resource_blocked_queues[3];
    
    // Memory viewer
    GtkWidget *memory_view;
    
    // Log console
    GtkWidget *log_view;
    
    // Process creation
    GtkWidget *file_chooser;
    GtkWidget *add_process_button;
    
    // Timer for auto execution
    guint timer_id;
    gboolean is_running;
} AppWidgets;

void initialize_gui(int *argc, char ***argv);
void update_gui(AppWidgets *widgets);
void add_log_message(const char *message);

#endif