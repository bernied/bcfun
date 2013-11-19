#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_cl.h"
#include "bitcoin_blocks.h"
#include "sha256.h"
#include "util.h"

#define VERSION "0.1"

struct arg_t args;
//const char binary_chars[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

void
init_default_args(struct arg_t* args)
{
  args->h = false;    // display this help and exit
  args->v = false;    // output version information and exit
  args->x = false;    // print result in hex string
  args->b = false;    // print result as binary values
  args->c = false;    // treat file as json bitcoin block and double hash it
  args->n = 0;        // if bitcoin then iterate over nonce
  args->p = 8;        // number of bytes to print
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

  if (args->x == false && args->b == false) {
    args->x = true;
  }
  else if (args->x == true && args->b == true) {
    fprintf(stderr, "-x and -b can not be used at the same time\n");
    exit(EXIT_FAILURE);
  }

  if (args->c == false && args->n != 0) {
    fprintf(stderr, "Set nonce w/o setting bitcoin mode\n");
    exit(EXIT_FAILURE);
  }

  if (args->p < 1 || args->p > 8) {
    fprintf(stderr, "number of bytes to print must be betwee 1 and 8\n");
    exit(EXIT_FAILURE);
  }

  return argv[args->optind];
}

int
sha_256_bc_json(char* file_name, FILE* file, off_t size)
{
  int result =0;
  uint32 digest[16];
  unsigned char* json;
  block_header* block;
  uint32 block_digest[8];

  json = calloc(size+1, sizeof(unsigned char));
  if (json == NULL) 
  {
    fprintf(stderr, "Unable to allocate memory for json file %s.\n", file_name);
    exit(EXIT_FAILURE);
  }

  block = calloc(1024, sizeof (uint8));
  if (block == NULL)
  {
    fprintf(stderr, "Unable to allocate memory for block header.\n");
    exit(EXIT_FAILURE);
  }

  size_t len = read_file(file, json, size);
  if (len != size)
  {
    fprintf(stderr, "Failure reading file %s.\n", file_name);
    exit(EXIT_FAILURE);
  }
  result = parse_bitcoin_block(json, len, block);
  //print_block_header(block);

  prime_final_block((uint8*) block, 2, 0, sizeof(block_header));
  sha_256_hash((uint32*) block, 2, digest);
  //print_digest(digest, args.x);
  
  prime_final_block((uint8*) digest, 1, 0, 32);
  sha_256_hash(digest, 1, block_digest);
  reverse_bytes((unsigned char*)block_digest, 32);
  print_digest(block_digest, args.x);

  free(json);
  return result;
}

int
sha_256_file(char* file_name, FILE* file, off_t size)
{
  int result;
  uint32 digest[8];

  // Initialize 512 bit blocks for processing
  uint32 num_blocks = (size / BLOCK_SIZE_BYTES) + (size % BLOCK_SIZE_BYTES == 0 ? 0 : 1);
  uint32 num_pad_blocks = BLOCK_SIZE_BITS - ((size << 3) % BLOCK_SIZE_BITS) < PAD_SIZE_BITS ? 1 : 0; // loosing some precision here
  uint32* blocks = (uint32*) calloc(num_blocks + num_pad_blocks, BLOCK_SIZE_BYTES);
  if (blocks == NULL) 
  {
    fprintf(stderr, "Unable to allocate memory for blocks from file %s.\n", file_name);
    exit(EXIT_FAILURE);
  }

  // Load up block data
  size_t total_bytes = read_file(file, (unsigned char*)blocks, size);
  if (total_bytes != size)
  {
    fprintf(stderr, "Unable to read entire file %s.\n", file_name);
    exit(EXIT_FAILURE);
  }

  // Set up final block
  prime_final_block((uint8*) blocks, num_blocks, num_pad_blocks, size);

  // Calculate and print the sha-256 digest of the incoming data
  sha_256_hash(blocks, num_blocks + num_pad_blocks, digest);
  print_digest(digest, args.x);

  free(blocks);
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

  if (args.c) {
    result = sha_256_bc_json(file_name, file, size);
  }
  else {
    result = sha_256_file(file_name, file, size);
  }

  fclose(file);
  exit(result);
}
