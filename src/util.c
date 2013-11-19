#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void
create_digest_str(uint8 digest[], char buffer[], const char hex_chars[16])
{
  const char* chars;
  if (hex_chars == NULL) {
    chars =  "0123456789abcdef";
  }
  else {
    chars = hex_chars;
  }

  uint8 *d = digest;

  for (int i = 0; i < 32; i++)
  {
    *buffer++ = chars[(*d & 0xF0) >> 4];
    *buffer++ = chars[*d & 0x0F];
    d++;
  }
  *buffer = (char)0;
}

void
print_digest(uint32* digest, bool hex)
{
  if (hex)
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
read_file(FILE* file, unsigned char* file_mem, size_t size)
{
  unsigned char buff[1024];
  int total=0, len;
  while((len = fread(buff, 1, 1024, file)) > 0 && total < size)
  {
    memcpy(file_mem+total, buff, len);
    total += len;
  }

  return total;
}

/*
size_t
load_block_array(uint32* blocks, uint32 num_blocks, uint32 block_size_bytes, FILE* file)
{
  printf("%p, %d, %d\n", blocks, num_blocks, block_size_bytes);
  size_t num_bytes =0;
  uint8* p = (uint8) blocks;
  for(int i =0; i < num_blocks; i++) {
    num_bytes = fread(p, 1, block_size_bytes, file);
    p += num_bytes;
  }

  return num_bytes;
}
*/

void
reverse_bytes(unsigned char* array, int len)
{
  unsigned char *l = array, *r = array + len -1;
  unsigned char tmp;
  while(l < r) {
    tmp = *l;
    *l++ = *r;
    *r-- = tmp;
  }
}

#ifndef HEXDUMP_COLS
#define HEXDUMP_COLS 8
#endif

/*
 * Code by graspus. See http://grapsus.net/blog/post/Hexadecimal-dump-in-C
 */
void
hexdump(void *mem, uint32 len)
{
  uint32 i, j;

  for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
  {
    /* print offset */
    if(i % HEXDUMP_COLS == 0)
    {
      printf("0x%06x: ", i);
    }

    /* print hex data */
    if(i < len)
    {
      printf("%02x ", 0xFF & ((char*)mem)[i]);
    }
    else /* end of block, just aligning for ASCII dump */
    {
      printf("   ");
    }

    /* print ASCII dump */
    if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
    {
      for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
      {
        if(j >= len) /* end of block, not really printing */
        {
          putchar(' ');
        }
        else if(isprint(((char*)mem)[j])) /* printable char */
        {
          putchar(0xFF & ((char*)mem)[j]);        
        }
        else /* other char */
        {
          putchar('.');
        }
      }
      putchar('\n');
    }
  }
}
 
