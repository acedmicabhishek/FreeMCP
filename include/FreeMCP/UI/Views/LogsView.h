#pragma once
#include <gtk/gtk.h>
#include "FreeMCP/UI/StoreApp.h"

namespace FreeMCP::UI::Views {
    GtkWidget* create_logs_view(StoreApp* app_data);
}
