#include <gtk/gtk.h>
#include "gui_helpers.h"

extern void activate(GtkApplication *app, gpointer user_data); // declare from scheduler_gui.c

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.csen602.scheduler", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}