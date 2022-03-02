

#ifndef __SPAW_PCAPNG_H_
#define __SPAW_PCAPNG_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct tag_ST_PCAPNG
{
   unsigned char byte_swapped;
   unsigned char *buffer;
   int current_pos;
} ST_PCAPNG;

int spaw_pcapng_open(char *in_filename, char *out_filename, void *data);

#endif
