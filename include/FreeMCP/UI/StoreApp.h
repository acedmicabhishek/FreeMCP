#pragma once
#include <gtk/gtk.h>
#include <string>

struct StoreApp {
    GtkWidget* stack;
    GtkWidget* sidebar_list;
    GtkWidget* ollama_list;
    GtkWidget* brain_list;
    GtkWidget* chat_history;
    GtkWidget* chat_entry;
    GtkWidget* log_view;
    GtkWidget* pull_entry;
    GtkWidget* active_brain_label;
    std::string active_model;
};


extern StoreApp* global_app;

#ifdef __cplusplus
extern "C" {
#endif
    void log_message(const char* msg);
#ifdef __cplusplus
}
#endif
