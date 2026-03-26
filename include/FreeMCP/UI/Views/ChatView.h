#pragma once
#include <gtk/gtk.h>
#include "FreeMCP/UI/StoreApp.h"

namespace FreeMCP::UI::Views {
    GtkWidget* create_chat_view(StoreApp* app_data);
    void append_to_chat(StoreApp* app_data, const char* sender, const char* message);
}
