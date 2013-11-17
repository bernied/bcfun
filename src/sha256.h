#ifndef __SHA_256_HASH__
#define __SHA_256_HASH__

#include "types.h"

#define BLOCK_SIZE 512
#define PAD_SIZE (1+64)

#define RR(x,s) (((x) >> (s)) | ((x) << (32 - (s))))
#define SR(x,s) ((x) >> (s))
#define CH(x,y,z) (((x) & (y)) ^ ((~(x)) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA_0(x) (RR((x),  2) ^ RR((x), 13) ^ RR((x), 22))
#define SIGMA_1(x) (RR((x),  6) ^ RR((x), 11) ^ RR((x), 25))
#define sigma_0(x) (RR((x),  7) ^ RR((x), 18) ^ SR((x), 3))
#define sigma_1(x) (RR((x), 17) ^ RR((x), 19) ^ SR((x), 10))

#define REVERSE_32(r, v) { \
  uint32 tmp = (((v) << 8) & 0xFF00FF00UL ) | (((v) >> 8) & 0xFF00FFUL ); \
  (r) = (tmp << 16) | (tmp >> 16); \
}

#define REVERSE_64(r, v) { \
  uint64 tmp = (((v) << 8) & 0xFF00FF00FF00FF00ULL ) | (((v) >> 8) & 0x00FF00FF00FF00FFULL ); \
  tmp = ((tmp << 16) & 0xFFFF0000FFFF0000ULL ) | ((tmp >> 16) & 0x0000FFFF0000FFFFULL ); \
  (r) = (tmp << 32) | (tmp >> 32); \
}

const static uint32 K[] = {
  0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 
  0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
  0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
  0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
  0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
  0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
  0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
  0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
  0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
  0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
  0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
  0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
  0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
  0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
  0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
  0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

extern struct arg_t args;

void sha_256_hash(uint32* blocks, uint32 num_blocks, uint32 digest[8], int reverse);
void create_digest_str(uint8 digest[], char buffer[], const char hex_chars[]);
void prime_final_block(uint8* blocks, uint32 num_blocks, uint32 size, uint32 last_block_size);

#endif
