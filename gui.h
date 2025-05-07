#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "memory.h"
#include "pcb.h"
#include "scheduler.h"
#include "mutex.h"
#include "queue.h"

// Global GTK widgets
typedef struct
{
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

// Function prototypes
void initialize_gui(int *argc, char ***argv);
void update_gui(AppWidgets *widgets);
void add_log_message(const char *format, ...);
void update_process_list(AppWidgets *w);
void update_queues(AppWidgets *w);
void update_mutex_status(AppWidgets *w);
void update_memory_view(AppWidgets *w);

// Callback function prototypes
void on_step_button_clicked(GtkButton *button, gpointer user_data);
void on_start_button_clicked(GtkButton *button, gpointer user_data);
void on_stop_button_clicked(GtkButton *button, gpointer user_data);
void on_reset_button_clicked(GtkButton *button, gpointer user_data);
void on_algorithm_changed(GtkComboBox *combo, gpointer user_data);
void on_quantum_changed(GtkSpinButton *spin, gpointer user_data);
void on_add_process_clicked(GtkButton *button, gpointer user_data);
gboolean auto_execute(gpointer data);
extern AppWidgets widgets;

#endif /* GUI_H */