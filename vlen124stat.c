/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Extracting the vlen information from trace in the original format (Yuehai's).
// It generates three files each containing a unique unit size.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 4) {
    printf("usage: %s <in1> <in2> <in4>\n", argv[0]);
    exit(0);
  }
  FILE * const in1 = fopen(argv[1], "rb"); assert(in1);
  FILE * const in2 = fopen(argv[2], "rb"); assert(in2);
  FILE * const in4 = fopen(argv[3], "rb"); assert(in4);
  uint64_t nr = 0;
  uint64_t sum = 0;

  uint8_t v1 = 0;
  uint16_t v2 = 0;
  uint32_t v4 = 0;
  while (fread(&v1, 1, 1, in1) == 1) { nr++; sum += v1; }
  while (fread(&v2, 2, 1, in2) == 1) { nr++; sum += v2; }
  while (fread(&v4, 4, 1, in4) == 1) { nr++; sum += v4; }
  printf("%.2f\n", ((double)sum / (double) nr));
  return 0;
}
