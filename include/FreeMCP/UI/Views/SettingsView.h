#pragma once
#include <gtk/gtk.h>
#include "FreeMCP/UI/StoreApp.h"

namespace FreeMCP::UI::Views {
    GtkWidget* create_settings_view(StoreApp* app_data);
    void refresh_ollama_list(StoreApp* app_data);
}
