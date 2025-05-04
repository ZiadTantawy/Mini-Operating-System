#include <gtk/gtk.h>
#include "gui_helpers.h"
#include "gui_handlers.h"


// Global Widgets
GtkWidget *process_list, *ready_queue, *blocked_queue, *running_process;
GtkWidget *memory_view, *mutex_status, *execution_log;
GtkWidget *scheduler_dropdown, *quantum_spin, *start_btn, *stop_btn, *reset_btn;

static void apply_css(GtkWidget *widget) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "* {\n"
        "  font-family: 'Times New Roman';\n"
        "  font-size: 14px;\n"
        "}\n"
        "window {\n"
        "  background: linear-gradient(to bottom right, #d8eaff, #f0d9ff);\n"
        "}\n"
        "frame {\n"
        "  background-color: rgba(255, 255, 255, 0.6);\n"
        "  border-radius: 8px;\n"
        "  padding: 10px;\n"
        "}\n"
        "label {\n"
        "  color: #333;\n"
        "}\n"
        "button {\n"
        "  background-color: #b3cfff;\n"
        "  color: #333;\n"
        "  border-radius: 12px;\n"
        "  padding: 8px 14px;\n"
        "  font-weight: bold;\n"
        "  transition: background 200ms;\n"
        "}\n"
        "button:hover {\n"
        "  background-color: #99b3ff;\n"
        "}\n",
        -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

GtkWidget* make_expandable(GtkWidget *widget) {
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_vexpand(widget, TRUE);
    return widget;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *grid, *frame, *box;

    // Window setup
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "OS Scheduler Simulation");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 700);
    apply_css(window);

    // Grid layout
    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 15);
    gtk_widget_set_hexpand(grid, TRUE);
    gtk_widget_set_vexpand(grid, TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Main Dashboard
    frame = gtk_frame_new("Main Dashboard");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    process_list = gtk_label_new("Processes will be listed here.");
    gtk_box_pack_start(GTK_BOX(box), process_list, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(box));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 0, 0, 2, 1);

    // Scheduler Control
    frame = gtk_frame_new("Scheduler Control");
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    scheduler_dropdown = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scheduler_dropdown), "FCFS");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scheduler_dropdown), "Round Robin");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(scheduler_dropdown), "Multilevel Feedback");

    quantum_spin = gtk_spin_button_new_with_range(1, 10, 1);
    start_btn = gtk_button_new_with_label("Start");
    stop_btn = gtk_button_new_with_label("Stop");
    reset_btn = gtk_button_new_with_label("Reset");

    gtk_box_pack_start(GTK_BOX(box), scheduler_dropdown, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), quantum_spin, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), start_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), stop_btn, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(box), reset_btn, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(box));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 2, 0, 1, 1);

    // Mutex Status
    frame = gtk_frame_new("Mutex Status");
    mutex_status = gtk_label_new("Resource usage info here.");
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(mutex_status));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 0, 1, 1, 1);

    // Memory Viewer
    frame = gtk_frame_new("Memory Viewer");
    memory_view = gtk_label_new("Memory view here.");
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(memory_view));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 1, 1, 1, 1);

    // Execution Log
    frame = gtk_frame_new("Execution Log");
    execution_log = gtk_label_new("Log messages here.");
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(execution_log));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 2, 1, 1, 1);

    // Add Process
    frame = gtk_frame_new("Add Process");
    GtkWidget *add_btn = gtk_button_new_with_label("Load Process File");
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(add_btn));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 0, 2, 1, 1);

    // Execution Control
    frame = gtk_frame_new("Execution Control");
    GtkWidget *step_btn = gtk_button_new_with_label("Step");
    GtkWidget *auto_btn = gtk_button_new_with_label("Auto Run");
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(box), step_btn, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(box), auto_btn, TRUE, TRUE, 5);
    gtk_container_add(GTK_CONTAINER(frame), make_expandable(box));
    gtk_grid_attach(GTK_GRID(grid), make_expandable(frame), 1, 2, 2, 1);

    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), window);
    g_signal_connect(stop_btn, "clicked", G_CALLBACK(on_stop_clicked), window);
    g_signal_connect(reset_btn, "clicked", G_CALLBACK(on_reset_clicked), window);
    g_signal_connect(step_btn, "clicked", G_CALLBACK(on_step_clicked), window);
    g_signal_connect(auto_btn, "clicked", G_CALLBACK(on_auto_clicked), window);
    g_signal_connect(scheduler_dropdown, "changed", G_CALLBACK(on_scheduler_changed), NULL);
    g_signal_connect(quantum_spin, "value-changed", G_CALLBACK(on_quantum_changed), NULL);
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_load_process_clicked), window);

    gtk_widget_show_all(window);
}
