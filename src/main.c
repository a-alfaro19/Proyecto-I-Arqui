#include <stdint.h>

// Assembly functions declaration
extern void tea_encrypt(uint32_t v[2], const uint32_t key[4]);
extern void tea_decrypt(uint32_t v[2], const uint32_t key[4]);

// Simple implementation of basic functions since we're in bare-metal environment
void print_char(char c) {
    // In a real bare-metal environment, this would write to UART
    // For now, this is just a placeholder
    volatile char *uart = (volatile char*)0x10000000;
    *uart = c;
}

void print_number(int num) {
    if (num == 0) {
        print_char('0');
        return;
    }
    
    if (num < 0) {
        print_char('-');
        num = -num;
    }
    
    char buffer[10];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    // Print digits in reverse order
    while (i > 0) {
        print_char(buffer[--i]);
    }
}

void print_string(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

void print_hex32(uint32_t num) {
    const char *hex = "0123456789ABCDEF";
    print_string("0x");
    for (int i = 28; i >= 0; i -= 4) {
        print_char(hex[(num >> i) & 0xF]);
    }
}

void print_hex64(uint64_t num) {
    const char *hex = "0123456789ABCDEF";
    print_string("0x");
    for (int i = 60; i >= 0; i -= 4) {
        print_char(hex[(num >> i) & 0xF]);
    }
}

void print_byte(uint8_t b) {
    const char *hex = "0123456789ABCDEF";
    print_char(hex[(b >> 4) & 0xF]);
    print_char(hex[b & 0xF]);
}

void print_bytes(const uint8_t *data, int len) {
    for (int i = 0; i < len; i++) {
        print_byte(data[i]);
        print_char(' ');  // espacio entre bytes
    }
    print_char('\n');
}

#define HEAP_SIZE 4096
static unsigned char heap[HEAP_SIZE];
static unsigned int heap_index = 0;

void *malloc(unsigned int size) {
  if (heap_index + size > HEAP_SIZE) {
    return 0;
  }

  void *ptr = &heap[heap_index];
  heap_index += size;

  return ptr;
}

void *calloc(unsigned int num, unsigned int size) {
  unsigned int total = num * size;
  unsigned char *ptr = malloc(total);

  if (!ptr) return 0;

  for (unsigned int i = 0; i < total; i++) {
    ptr[i] = 0;
  }

  return ptr;
}

void free(void *ptr) {
  (void)ptr;
}

void *memcpy(void *dest, const void *src, unsigned int n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

int strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}


// Cryptographic functions
uint8_t* apply_padding(const uint8_t *bytes, int len, int *out_len) {
  int pad = (len % 8 == 0) ? 0 : 8 - (len % 8);
  *out_len = len + pad;

  uint8_t *padded = calloc(*out_len, 1);
  if (!padded) return 0;

  memcpy(padded, bytes, len);

  return padded;
}

uint64_t* string_to_blocks64(const uint8_t *bytes, int len, int *out_blocks) {
  // Calculate number of blocks
  int n_blocks = len / 8;

  // Reserve space in memory
  uint64_t *blocks = malloc(n_blocks * sizeof(uint64_t));

  if (!blocks) {
    *out_blocks = 0;
    return 0;
  }

  // Create blocks (Big endian)
  for (int i = 0; i < n_blocks; i++) {
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

void print_ascii_from_blocks(uint64_t *blocks, int n_blocks) {
  for (int i = 0; i < n_blocks; i++) {
    uint32_t v0 = (uint32_t)(blocks[i] >> 32);
    uint32_t v1 = (uint32_t)(blocks[i] & 0xFFFFFFFF);

    // big-endian → extract 4 bytes from v0 and v1
    for (int j = 3; j >= 0; j--) {
      char c = (char)((v0 >> (8*j)) & 0xFF);
      if (c != '\0') {
          print_char(c);
      }
    }
    for (int j = 3; j >= 0; j--) {
      char c = (char)((v1 >> (8*j)) & 0xFF);
      if (c != '\0') {
          print_char(c);
      }
    }
  }
  print_char('\n');
}

void main(int argc, char *argv[]) {
  /* Test 1 - Single Block */
  // // Key
  // const uint32_t key[4] = {0x12345678, 0x9ABCDEF0, 0xFEDCBA98, 0x76543210};

  // // String
  // const char* str = "HOLA1234";

  /* Test 2 - Multiple Blocks */
  // Key
  const uint32_t key[4] = {0xA1B2C3D4, 0x5E6F7788, 0x99ABCDEF, 0x01234567};

  // String
  const char* str = "Mensaje de prueba para TEA";

  // Program Header
  print_string("=== Tiny Encryption Algorithm (TEA) ===\n");

  // Encoding section
  print_string("Encoding string: ");
  print_string(str);
  print_char('\n');

  // Transform ASCII to Bytes
  uint8_t *bytes = (uint8_t *)str;
  int len = strlen(str);

  // Apply padding
  int padded_len = 0;
  uint8_t *padded = apply_padding(bytes, len, &padded_len);

  // Divide in blocks of 64 bits
  int n_blocks = 0;
  uint64_t *blocks = string_to_blocks64(padded, padded_len, &n_blocks);

  // Encrypt blocks
  uint64_t *encoded_blocks = malloc(n_blocks * sizeof(uint64_t));
  for (int i = 0; i < n_blocks; i++) {
    uint64_t block = blocks[i];
    uint32_t v[2];
    v[0] = (uint32_t)(block >> 32);
    v[1] = (uint32_t)(block & 0xFFFFFFFF);

    tea_encrypt(v, key);
    encoded_blocks[i] = ((uint64_t)v[0] << 32) | v[1];
  
    // Show result
    print_string("Encrypted block ");
    print_number(i);
    print_string(": ");
    print_hex64(encoded_blocks[i]);
    print_char('\n');
  }

  // Decrypt blocks
  uint64_t *decoded_blocks = malloc(n_blocks * sizeof(uint64_t));
  for (int i = 0; i < n_blocks; i++) {
    uint32_t v[2];
    v[0] = (uint32_t)(encoded_blocks[i] >> 32);
    v[1] = (uint32_t)(encoded_blocks[i] & 0xFFFFFFFF);

    tea_decrypt(v, key);
    decoded_blocks[i] = ((uint64_t)v[0] << 32) | v[1];

    // Show result
    print_string("Decoded block ");
    print_number(i);
    print_string(": ");
    print_hex64(decoded_blocks[i]);
    print_char('\n');
  }

  // Compare result
  uint8_t *decoded_bytes = malloc(padded_len);
  print_string("Decoded string (ASCII): ");
  print_ascii_from_blocks(decoded_blocks, n_blocks);
  print_char('\n');
  print_string("Original string: ");
  print_string(str);
  print_char('\n');

  // Infinite loop to keep program running
  while (1) {
      __asm__ volatile ("nop");
  }
}