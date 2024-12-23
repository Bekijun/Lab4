#include <gtk/gtk.h>

// Callback functions for button clicks
void on_add_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry **entries = (GtkEntry **)data;
    const gchar *num1_text = gtk_entry_get_text(entries[0]);
    const gchar *num2_text = gtk_entry_get_text(entries[1]);
    
    double num1 = atof(num1_text);
    double num2 = atof(num2_text);
    double result = num1 + num2;

    char result_text[50];
    snprintf(result_text, 50, "%.2f", result);
    gtk_entry_set_text(entries[2], result_text);
}

void on_sub_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry **entries = (GtkEntry **)data;
    const gchar *num1_text = gtk_entry_get_text(entries[0]);
    const gchar *num2_text = gtk_entry_get_text(entries[1]);
    
    double num1 = atof(num1_text);
    double num2 = atof(num2_text);
    double result = num1 - num2;

    char result_text[50];
    snprintf(result_text, 50, "%.2f", result);
    gtk_entry_set_text(entries[2], result_text);
}

void on_mul_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry **entries = (GtkEntry **)data;
    const gchar *num1_text = gtk_entry_get_text(entries[0]);
    const gchar *num2_text = gtk_entry_get_text(entries[1]);
    
    double num1 = atof(num1_text);
    double num2 = atof(num2_text);
    double result = num1 * num2;

    char result_text[50];
    snprintf(result_text, 50, "%.2f", result);
    gtk_entry_set_text(entries[2], result_text);
}

void on_div_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry **entries = (GtkEntry **)data;
    const gchar *num1_text = gtk_entry_get_text(entries[0]);
    const gchar *num2_text = gtk_entry_get_text(entries[1]);
    
    double num1 = atof(num1_text);
    double num2 = atof(num2_text);
    char result_text[50];

    if (num2 != 0) {
        double result = num1 / num2;
        snprintf(result_text, 50, "%.2f", result);
    } else {
        snprintf(result_text, 50, "Error: Division by zero");
    }

    gtk_entry_set_text(entries[2], result_text);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *num1_entry, *num2_entry, *result_entry;
    GtkWidget *add_button, *sub_button, *mul_button, *div_button;

    GtkEntry *entries[3];

    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create grid layout
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create entry widgets
    num1_entry = gtk_entry_new();
    num2_entry = gtk_entry_new();
    result_entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(result_entry), FALSE);

    entries[0] = GTK_ENTRY(num1_entry);
    entries[1] = GTK_ENTRY(num2_entry);
    entries[2] = GTK_ENTRY(result_entry);

    // Add entries to the grid
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Number 1:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), num1_entry, 1, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Number 2:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), num2_entry, 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Result:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), result_entry, 1, 2, 2, 1);

    // Create buttons
    add_button = gtk_button_new_with_label("Add");
    sub_button = gtk_button_new_with_label("Subtract");
    mul_button = gtk_button_new_with_label("Multiply");
    div_button = gtk_button_new_with_label("Divide");

    // Connect signals to buttons
    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_button_clicked), entries);
    g_signal_connect(sub_button, "clicked", G_CALLBACK(on_sub_button_clicked), entries);
    g_signal_connect(mul_button, "clicked", G_CALLBACK(on_mul_button_clicked), entries);
    g_signal_connect(div_button, "clicked", G_CALLBACK(on_div_button_clicked), entries);

    // Add buttons to the grid
    gtk_grid_attach(GTK_GRID(grid), add_button, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), sub_button, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mul_button, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), div_button, 1, 4, 1, 1);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

