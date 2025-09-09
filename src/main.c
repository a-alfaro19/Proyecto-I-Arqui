#include <gtk/gtk.h>

#define WIDTH 1200
#define HEIGHT 768


uint8_t* apply_padding(const uint8_t *bytes, size_t len, size_t *out_len) {
  size_t pad = (len % 8 == 0) ? 0 : 8 - (len % 8);
  *out_len = len + pad;

  uint8_t *padded = calloc(*out_len, 1);
  if (!padded) return NULL;

  memcpy(padded, bytes, len);

  return padded;
}

uint64_t* string_to_blocks64(const uint8_t *bytes,  size_t len, size_t *out_blocks) {
  // Calculate number of blocks
  size_t n_blocks = len / 8;

  // Reserve memory
  uint64_t *blocks = malloc(n_blocks * sizeof(u_int64_t));

  if (!blocks) {
    *out_blocks = 0;
    return NULL;
  }

  // Create blocks (Big endian)
  for (size_t i = 0; i < n_blocks; i++) {
    uint64_t block = 0;
    for (int j = 0; j < 8; j++) {
      block <<= 8;
      block |= ((uint64_t)bytes[i*8 + j]);
    }
    blocks[i] = block;
  }

  // Set number of blocks
  *out_blocks = n_blocks;

  return blocks;
}

static void on_button_encrypt(GtkButton *btn, gpointer user_data) {
  GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(user_data);
  const char *msg = gtk_combo_box_text_get_active_text(combo);

  if (!msg) {
    g_print("No option selected!\n");
    return;
  }

  // ASCII to Bytes
  uint8_t *bytes = (const uint8_t *)msg;
  size_t len = strlen(msg);

  // Apply padding
  size_t padded_len = 0;
  uint8_t *padded = apply_padding(bytes, len, &padded_len);

  // Divide in blocks of 64 bits
  size_t n_blocks = 0;
  uint64_t *blocks = string_to_blocks64(padded, padded_len, &n_blocks);

  for  (size_t i = 0; i < n_blocks; i++) {
    g_print("Block %zu: 0x%016lX\n", i, blocks[i]);
  }

  free(padded);
  free(blocks);
  g_free((gchar *)msg);
}

static void on_button_decode(GtkButton *btn, gpointer user_data) {
  GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(user_data);
  const char *option = gtk_combo_box_text_get_active_text(combo);


  g_print("Decoding: %s\n", option);
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

  // Encrypt Combo Label
  GtkWidget *encrypt_combo_label = gtk_label_new("Selecciona una cadena para cifrar:");
  gtk_box_pack_start(GTK_BOX(vbox), encrypt_combo_label, FALSE, FALSE, 0);

  // Combo Box for Encryption
  GtkWidget *encrypt_combo = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(encrypt_combo), "HOLA1234");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(encrypt_combo), "Mensaje de prueba para TEA");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(encrypt_combo), "Hello, World!");
  gtk_combo_box_set_active(GTK_COMBO_BOX(encrypt_combo), 0);
  gtk_box_pack_start(GTK_BOX(vbox), encrypt_combo, FALSE, FALSE, 0);

  // Encrypt Button
  GtkWidget *btn_encrypt = gtk_button_new_with_label("Encrypt");
  g_signal_connect(btn_encrypt, "clicked", G_CALLBACK(on_button_encrypt), encrypt_combo);
  gtk_box_pack_start(GTK_BOX(vbox), btn_encrypt, FALSE, FALSE, 0);

  // Separator
  GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(vbox), sep, FALSE, FALSE, 0);

  // Decode Combo Label
  GtkWidget *decode_combo_label = gtk_label_new("Selecciona una cadena para descifrar:");
  gtk_box_pack_start(GTK_BOX(vbox), decode_combo_label, FALSE, FALSE, 0);

  // Combo Box for Encryption
  GtkWidget *decode_combo = gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(decode_combo), "HOLA1234");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(decode_combo), "Mensaje de prueba para TEA");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(decode_combo), "Hello, World!");
  gtk_combo_box_set_active(GTK_COMBO_BOX(decode_combo), 0);
  gtk_box_pack_start(GTK_BOX(vbox), decode_combo, FALSE, FALSE, 0);

  // Encrypt Button
  GtkWidget *btn_decode = gtk_button_new_with_label("Decode");
  g_signal_connect(btn_decode, "clicked", G_CALLBACK(on_button_decode), decode_combo);
  gtk_box_pack_start(GTK_BOX(vbox), btn_decode, FALSE, FALSE, 0);

  // Show window's widgets
  gtk_widget_show_all(window);

  // Run UI
  gtk_main();

  return 0;
}