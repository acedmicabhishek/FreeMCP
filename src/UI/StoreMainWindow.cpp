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

static void on_auth_clicked(GtkButton* btn, gpointer data) {
    GtkWidget** widgets = (GtkWidget**)data;
    GtkWidget* entry = widgets[0];
    GtkWidget* error_label = widgets[1];
    
    const char* password = gtk_entry_get_text(GTK_ENTRY(entry));
    if (!password || strlen(password) == 0) return;

    log_message("Attempting custom sudo-S elevation...");
    
    
    std::string check_cmd = "echo \"" + std::string(password) + "\" | sudo -S -v 2>&1";
    int res = system(check_cmd.c_str());

    if (res == 0) {
        log_message("Elevation SUCCESS. Relaunching...");
        char self_path[1024];
        ssize_t len = readlink("/proc/self/exe", self_path, sizeof(self_path)-1);
        if (len != -1) {
            self_path[len] = '\0';
            
            std::string run_cmd = "echo \"" + std::string(password) + "\" | sudo -S -E " + std::string(self_path) + " --store &";
            system(run_cmd.c_str());
            exit(0);
        }
    } else {
        log_message("Elevation FAILED.");
        gtk_label_set_text(GTK_LABEL(error_label), "Authentication Failed. Incorrect Password?");
        gtk_widget_show(error_label);
    }
}

static void on_activate(GtkApplication* app, gpointer user_data) {
    global_app = new StoreApp();
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_default_size(GTK_WINDOW(window), 950, 650);
    gtk_window_set_title(GTK_WINDOW(window), "FreeMCP Terminal Shell");

    
    if (geteuid() != 0) {
        GtkWidget* overlay = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
        gtk_widget_set_halign(overlay, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(overlay, GTK_ALIGN_CENTER);
        gtk_container_add(GTK_CONTAINER(window), overlay);

        GtkWidget* img = gtk_image_new_from_icon_name("dialog-password", GTK_ICON_SIZE_DIALOG);
        gtk_box_pack_start(GTK_BOX(overlay), img, FALSE, FALSE, 0);

        GtkWidget* title = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>Elevation Required</span>");
        gtk_box_pack_start(GTK_BOX(overlay), title, FALSE, FALSE, 0);

        GtkWidget* desc = gtk_label_new("FreeMCP needs root privileges to manage your system.\nPlease enter your password below:");
        gtk_label_set_justify(GTK_LABEL(desc), GTK_JUSTIFY_CENTER);
        gtk_box_pack_start(GTK_BOX(overlay), desc, FALSE, FALSE, 0);

        GtkWidget* entry = gtk_entry_new();
        gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE); 
        gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Password");
        gtk_widget_set_size_request(entry, 280, 40);
        gtk_box_pack_start(GTK_BOX(overlay), entry, FALSE, FALSE, 10);

        GtkWidget* error_label = gtk_label_new("");
        gtk_widget_set_name(error_label, "error-label"); 
        gtk_box_pack_start(GTK_BOX(overlay), error_label, FALSE, FALSE, 0);

        GtkWidget* auth_btn = gtk_button_new_with_label("Unlock FreeMCP");
        gtk_widget_set_size_request(auth_btn, 280, 45);
        
        
        GtkWidget** callback_data = new GtkWidget*[2];
        callback_data[0] = entry;
        callback_data[1] = error_label;
        g_signal_connect(auth_btn, "clicked", G_CALLBACK(on_auth_clicked), callback_data);
        g_signal_connect(entry, "activate", G_CALLBACK(on_auth_clicked), callback_data);

        gtk_box_pack_start(GTK_BOX(overlay), auth_btn, FALSE, FALSE, 10);

        gtk_widget_show_all(window);
        gtk_widget_hide(error_label);
        return;
    }


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
