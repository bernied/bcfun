#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse_cl.h"
#include "bitcoin_blocks.h"
#include "sha256.h"

#define VERSION "0.1"

struct arg_t args;
//const char binary_chars[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

void
print_digest(uint32* digest)
{
  if (args.x)
  {
    char buffer[65];
    create_digest_str((uint8*) digest, buffer, NULL);

    printf("%s\n", buffer);
  }
  else
  {
    uint16* dp16 = (uint16*) digest;
    uint16 d16;

    for (int i=0; i < 16; i++)
    {
      d16 = dp16[i];
      putchar((d16 >>  8) & 0xFF);
      putchar(d16 & 0xFF);
    }
  }
}

size_t
read_file(FILE* file, unsigned char* json, size_t size)
{
  unsigned char buff[1024];
  int total=0, len;
  while((len = fread(buff, 1, 1024, file)) > 0 && total < size)
  {
    memcpy(json+total, buff, len);
    total += len;
  }

  return total;
}

size_t
load_block_array(uint32* blocks, uint32 num_blocks, FILE* file)
{
  size_t num_bytes =0;
  for(int i =0; i < num_blocks; i++) {
    num_bytes = fread(&blocks[i], 1, BLOCK_SIZE, file);
  }
  return num_bytes;
}

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
main(int argc, char** argv)
{
  int result=0;
  struct stat sb;
  uint32 digest[8];
  char* fileName;

  // Handle arguments
  fileName = handle_arguments(argc, argv, &args);

  if (stat(fileName, &sb) == -1)
  {
    fprintf(stderr, "File %s does not exist.\n", fileName);
    exit(EXIT_FAILURE);
  }
  uint32 size = (uint32) sb.st_size;

  // Open file for processing
  FILE* file = fopen(fileName, "r");
  if (file == NULL) 
  {
    fprintf(stderr, "An error occured while opening file \"%s\".", fileName);
    exit(EXIT_FAILURE);
  }

  if (args.c)
  {
    unsigned char* json;
    block_header* block;
    uint32 block_digest[8];

    json = calloc(size+1, sizeof(unsigned char));
    if (json == NULL) 
    {
      fprintf(stderr, "Unable to allocate memory for json file %s.\n", fileName);
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
      fprintf(stderr, "Failure reading file %s.\n", fileName);
      exit(EXIT_FAILURE);
    }
    result = parse_bitcoin_block(json, len, block);
    print_block_header(block);

    prime_final_block((uint8*) block, 2, sizeof(block_header), sizeof(block_header) - 64);
    sha_256_hash((uint32*) block, 2, digest, true);
    print_digest(digest);

    prime_final_block((uint8*) digest, 1, 32, 32);
    sha_256_hash(digest, 1, block_digest, true);
    print_digest(block_digest);

    free(json);
  }
  else
  {
    // Initialize 512 bit blocks for processing
    uint32 num_blocks = size/BLOCK_SIZE + (size % BLOCK_SIZE == 0 ? 0 : 1);
    uint32 num_pad_blocks = BLOCK_SIZE - (size % BLOCK_SIZE) < PAD_SIZE ? 1 : 0;
    uint32* blocks = (uint32*) calloc(num_blocks + num_pad_blocks, BLOCK_SIZE);
    if (blocks == NULL) 
    {
      fprintf(stderr, "Unable to allocate memory for blocks from file %s.\n", fileName);
      exit(EXIT_FAILURE);
    }

    // Load up block data
    size_t num_final_bytes = load_block_array(blocks, num_blocks, file);

    // Set up final block
    prime_final_block((uint8*) blocks, num_blocks + num_pad_blocks, size, num_final_bytes);

    // Calculate and print the sha-256 digest of the incoming data
    sha_256_hash(blocks, num_blocks, digest, true);
    print_digest(digest);
  
    free(blocks);
  }

  fclose(file);

  exit(result);
}