#include <stdio.h>
#include <string.h>

#include "sha256.h"
#include "util.h"

void
prime_final_block(uint8* blocks, uint32 num_blocks, bool pad_blocks, uint32 file_byte_size)
{
  uint8 *last_block, *start;
  uint64 num_bits, reversed_num_bits;
  uint32 last_block_byte_size = file_byte_size % BLOCK_SIZE_BYTES;

  //printf("num_blocks =%u, file_byte_size=%u, last_block_byte_size=%u\n", num_blocks, file_byte_size, last_block_byte_size);

  if (!pad_blocks)
  {
    last_block = blocks + (BLOCK_SIZE_BYTES * (num_blocks-1));
    start = last_block + last_block_byte_size; // assumes byte boundries

    // zero everything out (but last byte) after last bit
    memset(start, 0, BLOCK_SIZE_BYTES - last_block_byte_size -1);

    // set 1 bit at end; assumes byte boundries
    *start++ = 0x80;

    // set the size at the end of the buffer in big endian form
    uint64 num_bits = file_byte_size * 8;
    REVERSE_64(reversed_num_bits, num_bits);
    uint64* last = (uint64*) last_block;
    last[7] = reversed_num_bits;

    //hexdump(blocks, BLOCK_SIZE_BYTES*num_blocks);
  }
  else
  {
    printf("LAMb: prime_final_block nye!!!!\n");
  }
}

void
sha_256_hash(uint32* blocks, uint32 num_blocks, uint32 digest[8])
{
  uint32 a, b, c, d, e, f, g, h;
  uint32 W[64];
  uint32* M = blocks;
  uint32 Z[8];
  uint32 H[] = {  0x6a09e667UL, 0xbb67ae85UL, 0x3c6ef372UL, 0xa54ff53aUL,
                  0x510e527fUL, 0x9b05688cUL, 0x1f83d9abUL, 0x5be0cd19UL  };
  for (int i = 0; i < num_blocks; i++)
  {
    // prepare Wt
    for (int t=0; t < 16; t++) {
      REVERSE_32(W[t], M[t]);
    }
    for (int t=16; t < 64; t++) {
      W[t] = sigma_1(W[t-2]) + W[t-7] + sigma_0(W[t-15]) + W[t-16];
//      print_wt(sigma_1(W[t-2]), sigma_0(W[t-15]));
    }

    // initialize the working vars
    a = H[0]; b = H[1]; c = H[2]; d = H[3];
    e = H[4]; f = H[5]; g = H[6]; h = H[7];

    // iterate over the data
    for (int t = 0; t < 64; t++)
    {
      uint32 T1 = h + SIGMA_1(e) + CH(e, f, g) + K[t] + W[t];
      uint32 T2 = SIGMA_0(a) + MAJ(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
    }

    // assign the working vars for the next cycle
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;


    // go to next block of data
    M += 16;
  }

  // assign digest the final values
  for (int i=0; i < 8; i++) {
    REVERSE_32(digest[i], H[i]);
  }
}

