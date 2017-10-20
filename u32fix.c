/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

  int
main(int argc, char ** argv)
{
  if (argc != 2) {
    printf("Usage: %s <tracefile>\n", argv[0]);
    exit(0);
  }
  FILE * const fin = fopen(argv[1], "r");
  if (fin == NULL) exit(0);

  uint32_t * const keys = malloc(sizeof(uint32_t) * 65536);
  uint32_t max = 0;
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 65536, fin);
    for (size_t i = 0; i < nkeys; i++) {
      if (keys[i] > max) max = keys[i];
    }
    if (nkeys == 0) break;
  }
  printf("max %u\n", max);
  fseek(fin, -4, SEEK_END);
  fread(keys, sizeof(keys[0]), 1, fin);
  printf("last %u\n", keys[0]);
  if (max == keys[0]) {
    exit(0);
  }
  fclose(fin);
  FILE * const fapp = fopen(argv[1], "a");
  fwrite(&max, sizeof(max), 1, fapp);
  fflush(fapp);
  fclose(fapp);
  return 0;
}
