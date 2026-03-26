#include "FreeMCP/UI/Views/BrainView.h"
#include "FreeMCP/Ollama.h"
#include <vector>

namespace FreeMCP::UI::Views {

void refresh_brain_list(StoreApp* app_data) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(app_data->brain_list));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
    std::vector<std::string> models = Ollama::list_models();
    for (const auto& m : models) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(hbox), 12);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(m.c_str()), TRUE, TRUE, 0);
        if (m == app_data->active_model) {
            GtkWidget* badge = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(badge), "<span color='green'>ACTIVE</span>");
            gtk_box_pack_start(GTK_BOX(hbox), badge, FALSE, FALSE, 0);
        } else {
            GtkWidget* act_btn = gtk_button_new_with_label("Activate");
            g_object_set_data_full(G_OBJECT(act_btn), "model-name", g_strdup(m.c_str()), g_free);
            g_signal_connect_swapped(act_btn, "clicked", G_CALLBACK(+[](StoreApp* app, GtkWidget* btn) {
                const char* name = (const char*)g_object_get_data(G_OBJECT(btn), "model-name");
                gtk_label_set_markup(GTK_LABEL(app->active_brain_label), "Initializing...");
                log_message(("Activating Brain: " + std::string(name)).c_str());
                if (Ollama::run_model(name)) {
                    app->active_model = name;
                    std::string markup = "Active Brain: <span weight='bold' color='#3498db'>" + app->active_model + "</span>";
                    gtk_label_set_markup(GTK_LABEL(app->active_brain_label), markup.c_str());
                    log_message("Brain Activated Successfully.");
                } else {
                    gtk_label_set_markup(GTK_LABEL(app->active_brain_label), "<span color='red'>Activation Failed</span>");
                    log_message("CRITICAL: Failed to start Ollama service or load model.");
                }
                refresh_brain_list(app);
            }), app_data);
            gtk_box_pack_start(GTK_BOX(hbox), act_btn, FALSE, FALSE, 0);
        }
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_container_add(GTK_CONTAINER(app_data->brain_list), row);
    }
    gtk_widget_show_all(app_data->brain_list);
}

GtkWidget* create_brain_view(StoreApp* app_data) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 30);
    GtkWidget* head = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(head), "<span size='xx-large'>Brain Center</span>");
    gtk_box_pack_start(GTK_BOX(vbox), head, FALSE, FALSE, 0);
    app_data->active_brain_label = gtk_label_new("No Brain Active");
    gtk_box_pack_start(GTK_BOX(vbox), app_data->active_brain_label, FALSE, FALSE, 10);
    GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
    app_data->brain_list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scroll), app_data->brain_list);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    return vbox;
}

}
