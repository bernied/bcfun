#ifndef __UTIL__
#define __UTIL__

#include "types.h"

#define REVERSE_32(r, v) { \
  uint32 tmp = (((v) << 8) & 0xFF00FF00UL ) | (((v) >> 8) & 0xFF00FFUL ); \
  (r) = (tmp << 16) | (tmp >> 16); \
}

#define REVERSE_64(r, v) { \
  uint64 tmp = (((v) << 8) & 0xFF00FF00FF00FF00ULL ) | (((v) >> 8) & 0x00FF00FF00FF00FFULL ); \
  tmp = ((tmp << 16) & 0xFFFF0000FFFF0000ULL ) | ((tmp >> 16) & 0x0000FFFF0000FFFFULL ); \
  (r) = (tmp << 32) | (tmp >> 32); \
}

extern void print_digest(uint32* digest, bool hex);
extern size_t read_file(FILE* file, unsigned char* file_mem, size_t size);
extern void reverse_bytes(unsigned char* array, int len);
extern size_t load_block_array(uint32* blocks, uint32 num_blocks, uint32 block_size, FILE* file);
extern void create_digest_str(uint8 digest[], char buffer[], const char hex_chars[16]);
extern void hexdump(void *mem, uint32 len);

#endif
