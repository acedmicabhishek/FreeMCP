#include "FreeMCP/UI/StoreApp.h"
#include "FreeMCP/UI/Views/HomeView.h"
#include "FreeMCP/UI/Views/BrainView.h"
#include "FreeMCP/UI/Views/ChatView.h"
#include "FreeMCP/UI/Views/LogsView.h"
#include "FreeMCP/UI/Views/SettingsView.h"
#include "FreeMCP/Ollama.h"

using namespace FreeMCP::UI::Views;

static void on_row_selected(GtkListBox* listbox, GtkListBoxRow* row, gpointer user_data) {
    StoreApp* app_data = (StoreApp*)user_data;
    if (!row) return;
    int index = gtk_list_box_row_get_index(row);
    const char* names[] = {"home", "brain", "chat", "logs", "store", "settings"};
    if (index >= 0 && index < 6) {
        gtk_stack_set_visible_child_name(GTK_STACK(app_data->stack), names[index]);
        if (index == 1) refresh_brain_list(app_data);
        if (index == 5) refresh_ollama_list(app_data);
    }
}

static GtkWidget* create_store_view() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("MCP Store Ready"), TRUE, TRUE, 0);
    return vbox;
}

static void on_activate(GtkApplication* app, gpointer user_data) {
    global_app = new StoreApp();
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 950, 650);
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);
    GtkWidget* side = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(side, 180, -1);
    gtk_box_pack_start(GTK_BOX(box), side, FALSE, FALSE, 0);
    global_app->sidebar_list = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(side), global_app->sidebar_list, TRUE, TRUE, 0);
    const char* items[] = {"Home", "Brain", "Chat", "Logs", "Store", "Settings"};
    for (int i = 0; i < 6; i++) {
        GtkWidget* row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(row), gtk_label_new(items[i]));
        gtk_container_add(GTK_CONTAINER(global_app->sidebar_list), row);
    }
    global_app->stack = gtk_stack_new();
    gtk_box_pack_start(GTK_BOX(box), global_app->stack, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_home_view(global_app), "home");
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_brain_view(global_app), "brain");
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_chat_view(global_app), "chat");
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_logs_view(global_app), "logs");
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_store_view(), "store");
    gtk_stack_add_named(GTK_STACK(global_app->stack), create_settings_view(global_app), "settings");
    g_signal_connect(global_app->sidebar_list, "row-selected", G_CALLBACK(on_row_selected), global_app);
    gtk_widget_show_all(window);
    
    Ollama::discover_system();
    log_message("FreeMCP Interface Initialized.");
}

void launch_store(int argc, char* argv[]) {
    GtkApplication* app = gtk_application_new("org.freemcp.store", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    char* dummy_argv[] = { argv[0], NULL };
    g_application_run(G_APPLICATION(app), 1, dummy_argv);
    g_object_unref(app);
}
