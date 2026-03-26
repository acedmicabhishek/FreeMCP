#include "FreeMCP/UI/Views/HomeView.h"

namespace FreeMCP::UI::Views {

GtkWidget* create_home_view(StoreApp* app_data) {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 30);
    GtkWidget* title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>FreeMCP Control Hub</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);
    GtkWidget* sw = gtk_switch_new();
    gtk_widget_set_halign(sw, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), sw, FALSE, FALSE, 0);
    return vbox;
}

}
