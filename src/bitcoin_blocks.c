#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#include "bitcoin_blocks.h"
#include "sha256.h"
#include "util.h"
#include "js0n.h"

#define MAP_CHAR(x) ((uint8) (((x) >= '0' && (x) <= '9' ? ((x) - '0') : \
                              ((x) >= 'a' && (x) <= 'f' ? ((x) - 'a' + 10) : 0xCC))))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

char* tokens[NUM_TOKENS] = 
  { "", 
    "hash", 
    "ver", 
    "prev_block", 
    "mrkl_root", 
    "time", 
    "bits", 
    "nonce", 
    "tx", 
    "mrkl_tree"
  };

enum block_tokens
parse_token(uint8* json, int offset, int len)
{
  char* tok = (char*) json + offset;
  tok[len] ='\0';  // LAMb

  for (int i=1; i < NUM_TOKENS; i++)
  {
    if (strcmp(tok, tokens[i]) == 0) {
      return i;
    }
  }

  return UNKNOWN;
}

int
is_valid_token(enum block_tokens tok)
{
  return tok >= HASH && tok <= NONCE;
}

void
hex_to_bytes(char* str, int len, uint8* data, int data_len)
{
  //printf("len=%d\n", len);
  //assert(len == data_len/2)
  char c;
  uint8 u;
  char *s = str, *e = str + len;

  while (s < e)
  {
    c = *s++;
    u = MAP_CHAR(c);
//    printf("%x", u);
    *data = u << 4;

    c = *s++;
    u = MAP_CHAR(c);
//    printf("%x", u);
    *data++ |= u;
  }
//  printf("\n");
}

void
hex_to_bytes_rev(char* str, int len, uint8* data, int data_len)
{
  //printf("len=%d\n", len);
  //assert(len == data_len/2)
  char c;
  uint8 u;
  char *s = str, *e = str + len -1;

  while (e >= s)
  {
    c = *e--;
    u = MAP_CHAR(c);
//    printf("%x", u);
    *data = u;

    c = *e--;
    u = MAP_CHAR(c);
//    printf("%x", u);
    *data++ |= (u << 4);
  }
//  printf("\n");
}

int
parse_int(char* str)
{
  int r, i = atoi(str);
  REVERSE_32(r, i);
  return r;
}

int
parse_bitcoin_block(unsigned char* json, size_t size, block_header* block)
{
  unsigned short* res = calloc(size, sizeof(unsigned short));
  if (res == NULL) {
    return -1;
  }
  int ret = js0n(json, size, res, size);
  enum block_tokens token = UNKNOWN;
  uint32 value;

  int i =0, processed = 0;
  while (processed < 7 & res[i] != 0)
  {
    int s = res[i++];
    int l = res[i++];
    token = parse_token(json, s, l);
    s = res[i++];
    l = res[i++];

    if (is_valid_token(token))
    {
      char* str = (char*)(json + s);
      str[l] ='\0';  // LAMb
//      printf("str=%s\n", str);

      switch(token)
      { 
        case HASH:
          break;

        case VERSION:
          value = parse_int(str);
          REVERSE_32(block->version, value);
          break;

        case PREV_BLOCK:
          hex_to_bytes_rev(str, l, block->prev_block, 256);
          break;

        case MRKL_ROOT:
          hex_to_bytes_rev(str, l, block->mrkl_root, 256);
          break;

        case SECS:
          value = parse_int(str);
          REVERSE_32(block->time, value);
          break;

        case BITS:
          value = parse_int(str);
          REVERSE_32(block->bits, value);
          break;

        case NONCE:
          value = parse_int(str);
          REVERSE_32(block->nonce, value);
          break;

        default:
          break;

      }
      processed++;
    }
  }

  free(res);
  return 0;
}

// Assumes block_header allocation is 1024 bits
void
process_bit_coin_block_inline(block_header* block, uint32* block_digest)
{
  uint8 digest[64];

  prime_final_block((uint8*) block, 2, 0, sizeof(block_header));
  sha_256_hash((uint32*) block, 2, (uint32*) digest);
  
  prime_final_block(digest, 1, 0, 32);
  sha_256_hash((uint32*) digest, 1, block_digest);
  reverse_bytes((unsigned char*) block_digest, 32);
}

#ifdef BROKEN
// this doesn't work yet
void
mine_bit_coin_block(block_header* block, unsigned int start, unsigned int end)
{
  uint32 mined_block[16];
  uint32 mined_block_digest[8];
  block_header* mined_header = (block_header*) mined_block;
  uint8 digest[64];

  memcpy(block, mined_block, sizeof(block_header));
  prime_final_block((uint8*) mined_block, 2, 0, sizeof(block_header));
  prime_final_block(digest, 1, 0, 32);
  end &= 0x00000000FFFFFFFFUL;
  for(long i=start; i < end; i++)
  {
    mined_header->nonce = (unsigned int) i;
    sha_256_hash(mined_block, 2, (uint32*) digest); // LAMb: This can be optimized to make one call to hash loop
    sha_256_hash((uint32*) digest, 1, mined_block_digest);
    reverse_bytes((unsigned char*) mined_block_digest, 32);
    printf("nonce=%lu\t", i);
    print_digest(mined_block_digest, true);
  }
}
#endif

bool
less_then_bits(uint8* block, int bit_count)
{
  for (int i=0; i < bit_count / 8; i++)
  {
    if (block[i] != 0) {
      return false;
    }
  }
  return true;
}

void
mine_bit_coin_block(block_header* block, unsigned int start, unsigned int end)
{
  uint32 mined_block[32];
  uint32 mined_block_digest[8];
  block_header* mined_header = (block_header*) mined_block;
  uint8 digest[64];

  end &= 0x00000000FFFFFFFFUL;
  for(long i=start; i < end; i++)
  {
    block->nonce = i;
    memcpy(mined_block, block, sizeof(block_header));
    process_bit_coin_block_inline((block_header*) mined_block, mined_block_digest);
    if (less_then_bits((uint8*) mined_block_digest, 32))
    {
      printf("nonce=%lu\t", i);
      print_digest(mined_block_digest, true);
    }
  }
}

void
print_block_header(block_header* h)
{
  printf("%u\n0x", h->version);
  uint8* block = (uint8*) h->prev_block;
  for (int i=0; i < 32; i++) {
    printf("%02x", block[i]);
  }
  printf("\n0x");
  block = (uint8*) h->mrkl_root;
  for (int i=0; i < 32; i++) {
    printf("%02x", block[i]);
  }
  printf("\n");
  printf("%8.0x\n", h->time);
  printf("%8.0x\n", h->bits);
  printf("%8.0x\n", h->nonce);
}
