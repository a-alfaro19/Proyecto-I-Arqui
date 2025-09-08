#include <gtk/gtk.h>

#define WIDTH 1200
#define HEIGHT 768

static void on_combo_changed(GtkComboBoxText *combo, gpointer user_data) {
  const char *option = gtk_combo_box_text_get_active_text(combo);
  g_print("You choose the string: %s\n", option ? option : "None");
}


int main(int argc, char *argv[]) {
  GtkWidget *window;

  // Initialize GTK
  gtk_init (&argc, &argv);
    
  // Create window
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "TEA Encryption");
  gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Main Vertical Container
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // Combo Label
  GtkWidget *combo_label = gtk_label_new("Selecciona una cadena para cifrar:");
  gtk_box_pack_start(GTK_BOX(vbox), combo_label, FALSE, FALSE, 0);

  // Combo Box
  GtkWidget *combo = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "HOLA1234");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Mensaje de prueba para TEA");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "Hello, World!");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 0);

  // Show window's widgets
  gtk_widget_show_all(window);

  // Run UI
  gtk_main();

  return 0;
}