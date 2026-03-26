#include "FreeMCP/UI/Views/ChatView.h"
#include "FreeMCP/Ollama.h"
#include <cstring>

namespace FreeMCP::UI::Views {

void append_to_chat(StoreApp* app_data, const char* sender, const char* message) {
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_data->chat_history));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    std::string formatted = std::string(sender) + ": " + std::string(message) + "\n\n";
    gtk_text_buffer_insert(buffer, &end, formatted.c_str(), -1);
    
    gtk_text_buffer_get_end_iter(buffer, &end);
    GtkTextMark* mark = gtk_text_buffer_get_insert(buffer);
    gtk_text_buffer_move_mark(buffer, mark, &end);
    gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(app_data->chat_history), mark, 0.0, TRUE, 0.0, 1.0);
}

static void on_chat_send(GtkButton* btn, gpointer user_data) {
    StoreApp* app_data = (StoreApp*)user_data;
    const char* entry_text = gtk_entry_get_text(GTK_ENTRY(app_data->chat_entry));
    if (entry_text && strlen(entry_text) > 0) {
        std::string prompt = entry_text;
        append_to_chat(app_data, "You", prompt.c_str());
        gtk_entry_set_text(GTK_ENTRY(app_data->chat_entry), "");
        
        log_message(("Sending prompt to " + app_data->active_model).c_str());
        std::string response = Ollama::chat(app_data->active_model, prompt);
        append_to_chat(app_data, "AI", response.c_str());
    }
}

GtkWidget* create_chat_view(StoreApp* app_data) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
    app_data->chat_history = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app_data->chat_history), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app_data->chat_history), GTK_WRAP_WORD_CHAR);
    gtk_container_add(GTK_CONTAINER(scroll), app_data->chat_history);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    app_data->chat_entry = gtk_entry_new();
    GtkWidget* btn = gtk_button_new_with_label("Send");
    gtk_box_pack_start(GTK_BOX(hbox), app_data->chat_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    g_signal_connect(btn, "clicked", G_CALLBACK(on_chat_send), app_data);
    g_signal_connect(app_data->chat_entry, "activate", G_CALLBACK(on_chat_send), app_data);
    return vbox;
}

}
