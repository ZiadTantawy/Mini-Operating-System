#include <gtk/gtk.h>

static void on_button_clicked(GtkButton *button, gpointer user_data)
{
    GtkLabel *label = GTK_LABEL(user_data);
    gtk_label_set_text(label, "Button was clicked!");
}

static void on_activate(GtkApplication *app, gpointer user_data)
{
    // Create window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "GTK App with Button");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create vertical box layout
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create label
    GtkWidget *label = gtk_label_new("Click the button below");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create button
    GtkWidget *button = gtk_button_new_with_label("Click Me");
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

    // Connect button signal
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), label);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("com.example.GtkApp", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
