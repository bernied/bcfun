#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#include "bitcoin_blocks.h"
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
          value = atoi(str);
          block->version = value;
          break;

        case PREV_BLOCK:
          hex_to_bytes(str, l, block->prev_block, 256);
          break;

        case MRKL_ROOT:
          hex_to_bytes(str, l, block->mrkl_root, 256);
          break;

        case SECS:
          value = atoi(str);
          block->time = value;
          break;

        case BITS:
          value = atoi(str);
          block->bits = value;
          break;

        case NONCE:
          value = atoi(str);
          block->nonce = value;
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
  printf("%u\n", h->time);
  printf("%u\n", h->bits);
  printf("%u\n", h->nonce);
}
