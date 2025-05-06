#include "gui.h"
#include "queue.h"
#include "processLoader.h"
// Global widgets structure
AppWidgets widgets;
GtkTextBuffer *log_buffer;

// Function to add a message to the log
void add_log_message(const char *message)
{
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(log_buffer, &iter);
    gtk_text_buffer_insert(log_buffer, &iter, message, -1);
    gtk_text_buffer_insert(log_buffer, &iter, "\n", 1);
}

// Update process list
void update_process_list(AppWidgets *w)
{
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w->process_list)));
    gtk_list_store_clear(store);

    // Iterate through memory to find PCBs
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        if (strstr(memory[i].name, "PCB_") == memory[i].name)
        {
            if (strcmp(memory[i].name, "PCB_pid") == 0)
            {
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);

                int pid = atoi(memory[i].data);
                int state = -1;
                int priority = -1;
                int pc = -1;
                int start = -1;
                int end = -1;

                // Get other PCB fields
                for (int j = i; j < i + 6; j++)
                {
                    if (strcmp(memory[j].name, "PCB_state") == 0)
                    {
                        if (strcmp(memory[j].data, "NEW") == 0)
                            state = NEW;
                        else if (strcmp(memory[j].data, "READY") == 0)
                            state = READY;
                        else if (strcmp(memory[j].data, "RUNNING") == 0)
                            state = RUNNING;
                        else if (strcmp(memory[j].data, "BLOCKED") == 0)
                            state = BLOCKED;
                        else if (strcmp(memory[j].data, "TERMINATED") == 0)
                            state = TERMINATED;
                    }
                    else if (strcmp(memory[j].name, "PCB_priority") == 0)
                    {
                        priority = atoi(memory[j].data);
                    }
                    else if (strcmp(memory[j].name, "PCB_pc") == 0)
                    {
                        pc = atoi(memory[j].data);
                    }
                    else if (strcmp(memory[j].name, "PCB_start") == 0)
                    {
                        start = atoi(memory[j].data);
                    }
                    else if (strcmp(memory[j].name, "PCB_end") == 0)
                    {
                        end = atoi(memory[j].data);
                    }
                }

                // Add to list store
                gtk_list_store_set(store, &iter,
                                   0, pid,
                                   1, stateToString(state),
                                   2, priority,
                                   3, pc,
                                   4, start,
                                   5, end,
                                   -1);
            }
        }
    }
}

// Update queue displays
void update_queues(AppWidgets *w)
{
    // Clear current displays
    gtk_label_set_text(GTK_LABEL(w->ready_queue), "");
    gtk_label_set_text(GTK_LABEL(w->blocked_queue), "");
    gtk_label_set_text(GTK_LABEL(w->running_process), "");

    // Update ready queue
    GString *ready_str = g_string_new("Ready Queue:\n");
    Node *current = readyQueue.front;
    while (current != NULL)
    {
        g_string_append_printf(ready_str, "PID: %d, Priority: %d\n",
                               current->pcb.pid, current->pcb.priority);
        current = current->next;
    }
    gtk_label_set_text(GTK_LABEL(w->ready_queue), ready_str->str);
    g_string_free(ready_str, TRUE);

    // Update blocked queue
    GString *blocked_str = g_string_new("Blocked Queue:\n");
    current = blockedQueue.front;
    while (current != NULL)
    {
        g_string_append_printf(blocked_str, "PID: %d, Priority: %d\n",
                               current->pcb.pid, current->pcb.priority);
        current = current->next;
    }
    gtk_label_set_text(GTK_LABEL(w->blocked_queue), blocked_str->str);
    g_string_free(blocked_str, TRUE);

    // Update running process (simplified - in real implementation you'd track this)
    if (!isEmpty(&readyQueue))
    {
        PCB *running = peek(&readyQueue);
        if (running && running->state == RUNNING)
        {
            GString *running_str = g_string_new("");
            g_string_append_printf(running_str, "Running Process:\nPID: %d\nPC: %d\nPriority: %d",
                                   running->pid, running->programCounter, running->priority);
            gtk_label_set_text(GTK_LABEL(w->running_process), running_str->str);
            g_string_free(running_str, TRUE);
        }
    }
}

// Update mutex status
void update_mutex_status(AppWidgets *w)
{
    const char *mutex_names[] = {"userInput", "userOutput", "file"};
    Mutex *mutexes[] = {&userInputMutex, &userOutputMutex, &fileMutex};

    for (int i = 0; i < 3; i++)
    {
        GString *status = g_string_new("");
        g_string_append_printf(status, "%s Mutex: ", mutex_names[i]);

        if (mutexes[i]->isLocked)
        {
            g_string_append_printf(status, "Locked by PID %d\n", mutexes[i]->owner->pid);

            // Show blocked queue for this resource
            GString *blocked = g_string_new("Blocked Processes:\n");
            Node *current = mutexes[i]->blockedQueue.front;
            while (current != NULL)
            {
                g_string_append_printf(blocked, "PID: %d (Priority: %d)\n",
                                       current->pcb.pid, current->pcb.priority);
                current = current->next;
            }
            gtk_label_set_text(GTK_LABEL(w->resource_blocked_queues[i]), blocked->str);
            g_string_free(blocked, TRUE);
        }
        else
        {
            g_string_append(status, "Unlocked\n");
            gtk_label_set_text(GTK_LABEL(w->resource_blocked_queues[i]), "No processes waiting");
        }

        gtk_label_set_text(GTK_LABEL(w->mutex_status[i]), status->str);
        g_string_free(status, TRUE);
    }
}

// Update memory view
void update_memory_view(AppWidgets *w)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(w->memory_view));
    gtk_text_buffer_set_text(buffer, "", -1);

    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);

    gtk_text_buffer_insert(buffer, &iter, "Memory Contents:\n", -1);
    gtk_text_buffer_insert(buffer, &iter, "Addr\tName\t\tData\n", -1);
    gtk_text_buffer_insert(buffer, &iter, "--------------------------------\n", -1);

    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        char line[100];
        if (strlen(memory[i].name) > 0)
        {
            snprintf(line, sizeof(line), "%d\t%s\t\t%s\n", i, memory[i].name, memory[i].data);
        }
        else
        {
            snprintf(line, sizeof(line), "%d\t(free)\n", i);
        }
        gtk_text_buffer_insert(buffer, &iter, line, -1);
    }
}

// Update all GUI elements
void update_gui(AppWidgets *w)
{
    // Update clock cycle
    char cycle_str[50];
    snprintf(cycle_str, sizeof(cycle_str), "Clock Cycle: %d", clockCycle);
    gtk_label_set_text(GTK_LABEL(w->clock_cycle_label), cycle_str);

    // Update algorithm
    const char *algo_str = "Unknown";
    switch (currentAlgorithm)
    {
    case FCFS:
        algo_str = "First Come First Serve";
        break;
    case RR:
        algo_str = "Round Robin";
        break;
    case MLFQ:
        algo_str = "Multilevel Feedback Queue";
        break;
    }
    gtk_label_set_text(GTK_LABEL(w->algorithm_label), algo_str);

    // Update all sections
    update_process_list(w);
    update_queues(w);
    update_mutex_status(w);
    update_memory_view(w);
}

// Callback for step button
void on_step_button_clicked(GtkButton *button, gpointer user_data)
{
    scheduleOneInstruction();
    
    // Increment clock cycle after each instruction execution
    clockCycle++;
    
    update_gui(&widgets);
    add_log_message("Executed one instruction");
}

// Called every 1000ms when "Start" is clicked
gboolean auto_execute(gpointer data)
{
    scheduleFullProcess();
    
    // Increment clock cycle after each full process execution
    clockCycle++;
    
    update_gui(&widgets);

    if (isEmpty(&readyQueue) && runningPCB.pid == 0)
    {
        add_log_message("No more processes. Stopping.");
        widgets.is_running = FALSE;
        widgets.timer_id = 0;
        return FALSE;
    }

    add_log_message("Executed one full process or quantum");
    return TRUE;
}

// Callback for "Start" button
void on_start_button_clicked(GtkButton *button, gpointer user_data)
{
    if (!widgets.is_running)
    {
        widgets.is_running = TRUE;
        widgets.timer_id = g_timeout_add(1000, auto_execute, NULL);
        add_log_message("Started automatic execution");
    }
}

// Callback for stop button
void on_stop_button_clicked(GtkButton *button, gpointer user_data)
{
    if (widgets.is_running)
    {
        g_source_remove(widgets.timer_id);
        widgets.is_running = FALSE;
        add_log_message("Stopped automatic execution");
    }
}

// Callback for reset button
void on_reset_button_clicked(GtkButton *button, gpointer user_data)
{
    // Reset all queues and memory
    initSchedulers();
    initializeMemory();
    initMutexes();
    clockCycle = 0;

    update_gui(&widgets);
    add_log_message("System reset");
}

// Callback for algorithm change
void on_algorithm_changed(GtkComboBox *combo, gpointer user_data)
{
    gint active = gtk_combo_box_get_active(combo);
    currentAlgorithm = (SchedulingAlgorithm)active;
    update_gui(&widgets);
    add_log_message("Changed scheduling algorithm");
}

// Callback for quantum change
void on_quantum_changed(GtkSpinButton *spin, gpointer user_data)
{
    quantumNumber = gtk_spin_button_get_value_as_int(spin);
    add_log_message("Quantum changed");
}

// Callback for add process button
void on_add_process_clicked(GtkButton *button, gpointer user_data)
{
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widgets.file_chooser));
    if (filename)
    {
        static int pid_counter = 1;
        int result = loadProcess(filename, pid_counter++);
        if (result != -1)
        {
            char msg[100];
            snprintf(msg, sizeof(msg), "Loaded process from %s with PID %d", filename, pid_counter - 1);
            add_log_message(msg);
            update_gui(&widgets);
        }
        g_free(filename);
    }
}

// Initialize the GUI
void initialize_gui(int *argc, char ***argv)
{
    gtk_init(argc, argv);

    // Create main window
    widgets.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets.window), "OS Scheduler Simulation");
    gtk_window_set_default_size(GTK_WINDOW(widgets.window), 1200, 800);
    g_signal_connect(widgets.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create notebook for tabs
    widgets.notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(widgets.window), widgets.notebook);

    // ========== Dashboard & Control Panel Tab ==========
    GtkWidget *dashboard = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook), dashboard, gtk_label_new("Dashboard & Control"));

    // Top section - split into two columns
    GtkWidget *top_section = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(dashboard), top_section, FALSE, FALSE, 5);

    // Left column - Overview info
    GtkWidget *overview_frame = gtk_frame_new("System Status");
    GtkWidget *overview_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(overview_frame), overview_box);

    widgets.clock_cycle_label = gtk_label_new("Clock Cycle: 0");
    widgets.algorithm_label = gtk_label_new("Algorithm: First Come First Serve");
    gtk_box_pack_start(GTK_BOX(overview_box), widgets.clock_cycle_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(overview_box), widgets.algorithm_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(top_section), overview_frame, TRUE, TRUE, 0);

    // Right column - Control panel
    GtkWidget *control_frame = gtk_frame_new("Control Panel");
    GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(control_frame), control_box);

    // Algorithm selection
    GtkWidget *algo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *algo_label = gtk_label_new("Algorithm:");
    widgets.algorithm_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets.algorithm_combo), "First Come First Serve");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets.algorithm_combo), "Round Robin");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(widgets.algorithm_combo), "Multilevel Feedback Queue");
    gtk_combo_box_set_active(GTK_COMBO_BOX(widgets.algorithm_combo), 0);
    g_signal_connect(widgets.algorithm_combo, "changed", G_CALLBACK(on_algorithm_changed), NULL);

    // Quantum adjustment
    GtkWidget *quantum_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *quantum_label = gtk_label_new("Quantum:");
    widgets.quantum_spin = gtk_spin_button_new_with_range(1, 10, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widgets.quantum_spin), quantumNumber);
    g_signal_connect(widgets.quantum_spin, "value-changed", G_CALLBACK(on_quantum_changed), NULL);

    // Control buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    widgets.start_button = gtk_button_new_with_label("Start");
    widgets.stop_button = gtk_button_new_with_label("Stop");
    widgets.reset_button = gtk_button_new_with_label("Reset");
    widgets.step_button = gtk_button_new_with_label("Step");

    g_signal_connect(widgets.start_button, "clicked", G_CALLBACK(on_start_button_clicked), NULL);
    g_signal_connect(widgets.stop_button, "clicked", G_CALLBACK(on_stop_button_clicked), NULL);
    g_signal_connect(widgets.reset_button, "clicked", G_CALLBACK(on_reset_button_clicked), NULL);
    g_signal_connect(widgets.step_button, "clicked", G_CALLBACK(on_step_button_clicked), NULL);

    // Pack algorithm selection
    gtk_box_pack_start(GTK_BOX(algo_box), algo_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(algo_box), widgets.algorithm_combo, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(control_box), algo_box, FALSE, FALSE, 5);

    // Pack quantum spinner
    gtk_box_pack_start(GTK_BOX(quantum_box), quantum_label, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(quantum_box), widgets.quantum_spin, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(control_box), quantum_box, FALSE, FALSE, 5);

    // Pack buttons
    gtk_box_pack_start(GTK_BOX(button_box), widgets.start_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(button_box), widgets.stop_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(button_box), widgets.reset_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(button_box), widgets.step_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(control_box), button_box, FALSE, FALSE, 5);

    // Add the control frame to the top section
    gtk_box_pack_start(GTK_BOX(top_section), control_frame, TRUE, TRUE, 0);

    // Process list (using tree view)
    GtkWidget *process_frame = gtk_frame_new("Process List");
    GtkWidget *process_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(process_scroll, -1, 200);  // Set minimum height
    widgets.process_list = gtk_tree_view_new();

    // Create columns
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("PID", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("State", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Priority", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("PC", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Mem Start", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Mem End", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(widgets.process_list), column);

    // Create list store
    GtkListStore *store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(widgets.process_list), GTK_TREE_MODEL(store));
    g_object_unref(store);

    gtk_container_add(GTK_CONTAINER(process_scroll), widgets.process_list);
    gtk_container_add(GTK_CONTAINER(process_frame), process_scroll);
    gtk_box_pack_start(GTK_BOX(dashboard), process_frame, TRUE, TRUE, 5);

    // Queue section
    GtkWidget *queue_frame = gtk_frame_new("Queues");
    GtkWidget *queue_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    widgets.ready_queue = gtk_label_new("Ready Queue: Empty");
    widgets.blocked_queue = gtk_label_new("Blocked Queue: Empty");
    widgets.running_process = gtk_label_new("Running Process: None");

    gtk_box_pack_start(GTK_BOX(queue_box), widgets.ready_queue, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(queue_box), widgets.blocked_queue, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(queue_box), widgets.running_process, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(queue_frame), queue_box);
    gtk_box_pack_start(GTK_BOX(dashboard), queue_frame, FALSE, FALSE, 5);

    // ========== Resource Panel Tab ==========
    GtkWidget *resource_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook), resource_panel, gtk_label_new("Resources"));

    const char *resource_names[] = {"User Input", "User Output", "File Access"};
    for (int i = 0; i < 3; i++)
    {
        GtkWidget *frame = gtk_frame_new(resource_names[i]);
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_add(GTK_CONTAINER(frame), box);

        widgets.mutex_status[i] = gtk_label_new("Status: Unlocked");
        widgets.resource_blocked_queues[i] = gtk_label_new("No processes waiting");

        gtk_box_pack_start(GTK_BOX(box), widgets.mutex_status[i], FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(box), widgets.resource_blocked_queues[i], FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(resource_panel), frame, FALSE, FALSE, 0);
    }

    // ========== Memory Viewer Tab ==========
    GtkWidget *memory_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook), memory_tab, gtk_label_new("Memory"));

    GtkWidget *mem_scroll = gtk_scrolled_window_new(NULL, NULL);
    widgets.memory_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(widgets.memory_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(widgets.memory_view), TRUE);
    gtk_container_add(GTK_CONTAINER(mem_scroll), widgets.memory_view);
    gtk_box_pack_start(GTK_BOX(memory_tab), mem_scroll, TRUE, TRUE, 0);

    // ========== Log & Console Tab ==========
    GtkWidget *log_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook), log_tab, gtk_label_new("Log"));

    GtkWidget *log_scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *log_view = gtk_text_view_new();
    log_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(log_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(log_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(log_view), TRUE);
    gtk_container_add(GTK_CONTAINER(log_scroll), log_view);
    gtk_box_pack_start(GTK_BOX(log_tab), log_scroll, TRUE, TRUE, 0);

    // ========== Process Creation Tab ==========
    GtkWidget *process_tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(widgets.notebook), process_tab, gtk_label_new("Processes"));

    widgets.file_chooser = gtk_file_chooser_button_new("Select Process File", GTK_FILE_CHOOSER_ACTION_OPEN);
    widgets.add_process_button = gtk_button_new_with_label("Add Process");
    g_signal_connect(widgets.add_process_button, "clicked", G_CALLBACK(on_add_process_clicked), NULL);

    gtk_box_pack_start(GTK_BOX(process_tab), widgets.file_chooser, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(process_tab), widgets.add_process_button, FALSE, FALSE, 0);

    // Initialize state
    widgets.is_running = FALSE;
    widgets.timer_id = 0;

    // Show all and update initial state
    gtk_widget_show_all(widgets.window);
    update_gui(&widgets);
    add_log_message("System initialized");
}