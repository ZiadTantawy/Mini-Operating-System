// // gui_handlers.c
// #include <gtk/gtk.h>
// #include <stdlib.h>
// #include <string.h>
// #include "gui_helpers.h"
// #include "scheduler/scheduler.c"
// #include "memory.h"
// #include "mutex.c"
// #include "scheduler/queue.c"
// #include "pcb.h"
// #include "processLoader.c"
// #include "interpreter.c"

// extern GtkWidget *scheduler_dropdown, *quantum_spin;
// extern GtkWidget *execution_log, *memory_view, *process_list;
// extern int auto_mode;

// char scheduler_type[32] = "FCFS";
// int quantum = 2;

// // Signal handler: Start
// void on_start_clicked(GtkButton *button, gpointer user_data) {
//     clear_all();
//     load_programs();
//     log_append("Programs loaded. Ready to run.");
//     update_gui_state();
// }

// // Signal handler: Reset
// void on_reset_clicked(GtkButton *button, gpointer user_data) {
//     reset_simulation();
//     log_append("Simulation reset.");
//     update_gui_state();
// }

// // Signal handler: Scheduler type changed
// void on_scheduler_changed(GtkComboBoxText *combo, gpointer user_data) {
//     const char *text = gtk_combo_box_text_get_active_text(combo);
//     if (text) {
//         strncpy(scheduler_type, text, sizeof(scheduler_type));
//         log_appendf("Scheduler changed to: %s", scheduler_type);
//     }
// }

// // Signal handler: Quantum value changed
// void on_quantum_changed(GtkSpinButton *spin, gpointer user_data) {
//     quantum = gtk_spin_button_get_value_as_int(spin);
//     log_appendf("Quantum set to: %d", quantum);
// }

// // Signal handler: Step execution
// void on_step_clicked(GtkButton *button, gpointer user_data) {
//     schedule_cycle(scheduler_type, quantum);
//     update_gui_state();
// }

// // Auto run callback
// static gboolean auto_runner(gpointer data) {
//     if (!auto_mode) return G_SOURCE_REMOVE;
//     schedule_cycle(scheduler_type, quantum);
//     update_gui_state();
//     return G_SOURCE_CONTINUE;
// }

// // Signal handler: Auto run clicked
// void on_auto_clicked(GtkButton *button, gpointer user_data) {
//     auto_mode = 1;
//     g_timeout_add(1000, auto_runner, NULL);
//     log_append("Auto mode started.");
// }

// // Signal handler: Stop auto mode
// void on_stop_clicked(GtkButton *button, gpointer user_data) {
//     auto_mode = 0;
//     log_append("Auto mode stopped.");
// }

// // Signal handler: Load process file
// void on_load_process_clicked(GtkButton *button, gpointer user_data) {
//     GtkWidget *dialog;
//     GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
//     dialog = gtk_file_chooser_dialog_new("Open File",
//                                          GTK_WINDOW(user_data),
//                                          action,
//                                          ("_Cancel"), GTK_RESPONSE_CANCEL,
//                                          ("_Open"), GTK_RESPONSE_ACCEPT,
//                                          NULL);

//     if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
//         char *filename;
//         GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
//         filename = gtk_file_chooser_get_filename(chooser);
//         load_process_from_file(filename);
//         log_appendf("Loaded process from file: %s", filename);
//         g_free(filename);
//     }
//     gtk_widget_destroy(dialog);
//     update_gui_state();
// }
