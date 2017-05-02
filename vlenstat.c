/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// stat the vlen file of uint32_t[].

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  uint32_t len;
  uint64_t n =0;
  uint64_t sum = 0;
  while (fread(&len,  sizeof(len), 1, stdin)) {
    sum+= len;
    n++;
  }
  printf("n %" PRIu64 " avg %lf\n", n, ((double)sum)/((double)n));
  return 0;
}
