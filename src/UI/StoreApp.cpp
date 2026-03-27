#include "FreeMCP/UI/StoreApp.h"
#include <ctime>

StoreApp* global_app = nullptr;

struct LogData {
    std::string message;
};

static gboolean log_idle_cb(gpointer user_data) {
    LogData* data = (LogData*)user_data;
    if (global_app && global_app->log_view) {
        GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(global_app->log_view));
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        
        std::time_t now = std::time(nullptr);
        char timestamp[10];
        std::strftime(timestamp, sizeof(timestamp), "[%H:%M:%S] ", std::localtime(&now));
        
        std::string formatted = std::string(timestamp) + data->message + "\n";
        gtk_text_buffer_insert(buffer, &end, formatted.c_str(), -1);
        
        gtk_text_buffer_get_end_iter(buffer, &end);
        GtkTextMark* mark = gtk_text_buffer_get_insert(buffer);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(global_app->log_view), mark, 0.0, TRUE, 0.0, 1.0);
    }
    delete data;
    return FALSE;
}

extern "C" void log_message(const char* msg) {
    LogData* data = new LogData{msg};
    g_idle_add(log_idle_cb, data);
}
