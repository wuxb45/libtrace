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
  FILE * fout [32];
  char name[1024];
  for (int i = 0; i < 32; i++) {
    sprintf(name, "%s.%02d", argv[1], i);
    fout[i] = fopen(name, "w");
  }

  uint32_t * const keys = malloc(sizeof(uint32_t) * 1024*1024);
  uint32_t max = 0;
  uint32_t tmp;
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 1024*1024, fin);
    if (nkeys == 0) break;
    for (size_t i = 0; i < nkeys; i++) {
      if (keys[i] > max) max = keys[i];
      tmp = keys[i] /32;
      fwrite(&tmp, sizeof(tmp), 1, fout[keys[i]%32]);
    }
  }
  tmp = (max / 32) + 1;
  for (int i = 0; i < 32; i++) {
    fwrite(&tmp, sizeof(tmp), 1, fout[i]);
    fclose(fout[i]);
  }
  fclose(fin);
  return 0;
}
