#include "FreeMCP/UI/Views/SettingsView.h"
#include "FreeMCP/UI/Views/BrainView.h"
#include "FreeMCP/Ollama.h"
#include "FreeMCP/Persona.h"
#include <vector>

namespace FreeMCP::UI::Views {

void refresh_ollama_list(StoreApp* app_data) {
    GList* children = gtk_container_get_children(GTK_CONTAINER(app_data->ollama_list));
    for (GList* iter = children; iter != NULL; iter = g_list_next(iter)) gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
    std::vector<std::string> models = Ollama::list_models();
    for (const auto& m : models) {
        GtkWidget* row = gtk_list_box_row_new();
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_pack_start(GTK_BOX(hbox), gtk_label_new(m.c_str()), TRUE, TRUE, 0);
        GtkWidget* del_btn = gtk_button_new_from_icon_name("edit-delete", GTK_ICON_SIZE_BUTTON);
        g_object_set_data_full(G_OBJECT(del_btn), "model-name", g_strdup(m.c_str()), g_free);
        g_signal_connect_swapped(del_btn, "clicked", G_CALLBACK(+[](StoreApp* app, GtkWidget* btn) {
            const char* name = (const char*)g_object_get_data(G_OBJECT(btn), "model-name");
            if (Ollama::remove_model(name)) { refresh_ollama_list(app); refresh_brain_list(app); }
        }), app_data);
        gtk_box_pack_start(GTK_BOX(hbox), del_btn, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(row), hbox);
        gtk_container_add(GTK_CONTAINER(app_data->ollama_list), row);
    }
    gtk_widget_show_all(app_data->ollama_list);
}

static void on_character_changed(GtkComboBoxText* combo, gpointer user_data) {
    gchar* active = gtk_combo_box_text_get_active_text(combo);
    if (!active) return;
    std::string character = active;
    g_free(active);
    
    if (character == "Light") Persona::set_character(Persona::Character::LIGHT);
    else if (character == "Ace") Persona::set_character(Persona::Character::ACE);
    else if (character == "Jarvis") Persona::set_character(Persona::Character::JARVIS);
    else if (character == "Friday") Persona::set_character(Persona::Character::FRIDAY);
    else if (character == "Partner") Persona::set_character(Persona::Character::PARTNER);
    else if (character == "Cool Friend") Persona::set_character(Persona::Character::COOL_FRIEND);
    
    log_message(("Persona Character set to: " + character).c_str());
}

static void on_mode_changed(GtkComboBoxText* combo, gpointer user_data) {
    gchar* active = gtk_combo_box_text_get_active_text(combo);
    if (!active) return;
    std::string mode = active;
    g_free(active);
    
    if (mode == "Chill") Persona::set_mode(Persona::Mode::CHILL);
    else if (mode == "Productive") Persona::set_mode(Persona::Mode::PRODUCTIVE);
    else if (mode == "HackerMode") Persona::set_mode(Persona::Mode::HACKERMODE);
    
    log_message(("Persona Mode set to: " + mode).c_str());
}

GtkWidget* create_settings_view(StoreApp* app_data) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    
    GtkWidget* char_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(char_box), gtk_label_new("Character:"), FALSE, FALSE, 0);
    GtkWidget* char_combo = gtk_combo_box_text_new();
    const char* chars[] = {"Ace", "Light", "Jarvis", "Friday", "Partner", "Cool Friend"};
    for (int i = 0; i < 6; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(char_combo), chars[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(char_combo), 0);
    gtk_box_pack_start(GTK_BOX(char_box), char_combo, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), char_box, FALSE, FALSE, 0);
    g_signal_connect(char_combo, "changed", G_CALLBACK(on_character_changed), app_data);
    
    GtkWidget* mode_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(mode_box), gtk_label_new("Mode:"), FALSE, FALSE, 0);
    GtkWidget* mode_combo = gtk_combo_box_text_new();
    const char* modes[] = {"Chill", "Productive", "HackerMode"};
    for (int i = 0; i < 3; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mode_combo), modes[i]);
    gtk_combo_box_set_active(GTK_COMBO_BOX(mode_combo), 0);
    gtk_box_pack_start(GTK_BOX(mode_box), mode_combo, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), mode_box, FALSE, FALSE, 0);
    g_signal_connect(mode_combo, "changed", G_CALLBACK(on_mode_changed), app_data);

    GtkWidget* divider = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(vbox), divider, FALSE, FALSE, 10);

    app_data->pull_entry = gtk_entry_new();
    GtkWidget* pull = gtk_button_new_with_label("Pull Model");
    gtk_box_pack_start(GTK_BOX(vbox), app_data->pull_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), pull, FALSE, FALSE, 0);
    GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
    app_data->ollama_list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scroll), app_data->ollama_list);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 10);
    return vbox;
}

}
