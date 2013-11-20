#ifndef __BITCOIN_BLOCKS__
#define __BITCOIN_BLOCKS__

#include "types.h"

/*
Version Block   version number    You upgrade the software and it specifies a new version 4
hashPrevBlock   256-bit hash of the previous block header   A new block comes in    32
hashMerkleRoot  256-bit hash based on all of the transactions in the block  A transaction is accepted   32
Time    Current timestamp as seconds since 1970-01-01T00:00 UTC Every few seconds   4
Bits    Current target in compact format    The difficulty is adjusted  4
Nonce   32-bit number (starts at 0) A hash is tried (increments)    4
*/

enum block_tokens { UNKNOWN=0, HASH, VERSION, PREV_BLOCK, MRKL_ROOT, SECS, BITS, NONCE, TX, MRKL_TREE };

typedef struct block_header
{
  uint32 version;
  uint8 prev_block[32];
  uint8 mrkl_root[32];
  uint32 time;
  uint32 bits;
  uint32 nonce;
} block_header;

#define NUM_TOKENS 10
extern char* tokens[NUM_TOKENS];

extern int parse_bitcoin_block(unsigned char* json, size_t size, block_header* block);
extern void process_bit_coin_block_inline(block_header* block, uint32* block_digest);
extern void mine_bit_coin_block(block_header* block, unsigned int start, unsigned int end);
extern void print_block_header(block_header* h);

#endif