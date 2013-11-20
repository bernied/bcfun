#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "parse_cl.h"
#include "bitcoin_blocks.h"
#include "sha256.h"
#include "util.h"

#define VERSION "0.1"

struct arg_t args;
//const char binary_chars[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

/*
h / help              flag        "  display this help and exit"
v / version           flag        "  output version information and exit"
b / bit-coin          flag        "  treat file as json bitcoin block and double hash it"
m / mine              flag        "  iterate through all nonces"
0 / print-binary      int         "  output num bytes as binary"
x / print-hex         flag        "  print result as hex string (default)"
*/
void
init_default_args(struct arg_t* args)
{
  args->h = false;
  args->v = false;
  args->b = false;
  args->m = false;
  args->s = 0;
  args->e = UINT_MAX;
  args->_0 = 0;
  args->x = true;
}

char*
handle_arguments(int argc, char** argv, struct arg_t* args)
{
  init_default_args(args);
  Cmdline(args, argc, argv);
  if (argc == 1 || args->optind+1 != argc) {
    usage(-1, argv[0]);
  }

  if (args->v) {
    printf("sha256 version %s\n", VERSION);
  }

  if (args->_0 != 0) {
    args->x = false;
  }
  else if (args->x == true && args->_0 != 0) {
    fprintf(stderr, "-x and -0 can not be used at the same time\n");
    exit(EXIT_FAILURE);
  }

  if (args->b == false && args->m == true) {
    args->b = true;
  }

  if (args->_0 < 0 || args->_0 > 8) {
    fprintf(stderr, "number of bytes to print must be betwee 1 and 8\n");
    exit(EXIT_FAILURE);
  }

  return argv[args->optind];
}

int
sha_256_bc_json(char* file_name, FILE* file, off_t size)
{
  int result =-1;
  uint32 digest[16];
  unsigned char* json =NULL;
  uint8* block =NULL;
  uint32 block_digest[8];

  json = calloc(size+1, sizeof(unsigned char));
  if (json == NULL) 
  {
    fprintf(stderr, "Unable to allocate memory for json file %s.\n", file_name);
    goto exit;
  }

  block = calloc(128, sizeof(uint8));
  if (block == NULL)
  {
    fprintf(stderr, "Unable to allocate memory for block header.\n");
    goto exit;
  }

  size_t len = read_file(file, json, size);
  if (len != size)
  {
    fprintf(stderr, "Failure reading file %s.\n", file_name);
    goto exit;
  }

  result = parse_bitcoin_block(json, len, (block_header*) block);
  if (args.m) {
    mine_bit_coin_block((block_header*) block, args.s, args.e);
  }
  else {
   process_bit_coin_block_inline((block_header*) block, block_digest);
   print_digest(block_digest, args.x);
  }
  result = 0;

exit:
  if (json) free(json);
  if (block) free(block);
  return result;
}

int
sha_256_file(char* file_name, FILE* file, off_t size)
{
  int result =-1;
  uint32 digest[8];

  // Initialize 512 bit blocks for processing
  uint32 num_blocks = (size / BLOCK_SIZE_BYTES) + (size % BLOCK_SIZE_BYTES == 0 ? 0 : 1);
  uint32 num_pad_blocks = BLOCK_SIZE_BITS - ((size << 3) % BLOCK_SIZE_BITS) < PAD_SIZE_BITS ? 1 : 0; // loosing some precision here
  uint32* blocks = (uint32*) calloc(num_blocks + num_pad_blocks, BLOCK_SIZE_BYTES);
  if (blocks == NULL) 
  {
    fprintf(stderr, "Unable to allocate memory for blocks from file %s.\n", file_name);
    goto exit;
  }

  // Load up block data
  size_t total_bytes = read_file(file, (unsigned char*)blocks, size);
  if (total_bytes != size)
  {
    fprintf(stderr, "Unable to read entire file %s.\n", file_name);
    goto exit;
  }

  // Set up final block
  prime_final_block((uint8*) blocks, num_blocks, num_pad_blocks, size);

  // Calculate and print the sha-256 digest of the incoming data
  sha_256_hash(blocks, num_blocks + num_pad_blocks, digest);
  print_digest(digest, args.x);
  result = 0;

exit:
  if (blocks) free(blocks);
  return result;
}

int
main(int argc, char** argv)
{
  int result;
  struct stat sb;
  char* file_name;

  // Handle arguments
  file_name = handle_arguments(argc, argv, &args);

  if (stat(file_name, &sb) == -1)
  {
    fprintf(stderr, "File %s does not exist.\n", file_name);
    exit(EXIT_FAILURE);
  }
  off_t size = sb.st_size;

  // Open file for processing
  FILE* file = fopen(file_name, "r");
  if (file == NULL) 
  {
    fprintf(stderr, "An error occured while opening file \"%s\".", file_name);
    exit(EXIT_FAILURE);
  }

  if (args.b) {
    result = sha_256_bc_json(file_name, file, size);
  }
  else {
    result = sha_256_file(file_name, file, size);
  }

  fclose(file);
  exit(result);
}
