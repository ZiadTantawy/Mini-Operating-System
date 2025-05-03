#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include <gtk/gtk.h>

void on_start_clicked(GtkButton *button, gpointer user_data);
void on_reset_clicked(GtkButton *button, gpointer user_data);
void on_scheduler_changed(GtkComboBoxText *combo, gpointer user_data);
void on_quantum_changed(GtkSpinButton *spin, gpointer user_data);
void on_step_clicked(GtkButton *button, gpointer user_data);
void on_auto_clicked(GtkButton *button, gpointer user_data);
void on_stop_clicked(GtkButton *button, gpointer user_data);
void on_load_process_clicked(GtkButton *button, gpointer user_data);

#endif
