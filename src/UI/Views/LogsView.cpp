#include "FreeMCP/UI/Views/LogsView.h"

namespace FreeMCP::UI::Views {

GtkWidget* create_logs_view(StoreApp* app_data) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>System Logger</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    GtkWidget* scroll = gtk_scrolled_window_new(NULL, NULL);
    app_data->log_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app_data->log_view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(app_data->log_view), TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), app_data->log_view);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    return vbox;
}

}
