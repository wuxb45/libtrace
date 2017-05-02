/*
 * Copyright (c) 2016  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Extracting the klen/vlen information from trace in the original format (Yuehai's).
// encoding for klen: plain u8 array
// encoding for vlen: 0: read next two-byte vlen; 1: read next four-byte vlen; other: itself

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 2) {
    printf("usage: %s <out-prefix>\n", argv[0]);
    exit(0);
  }
  char filename[4096];

  sprintf(filename, "%s-klen", argv[1]);
  FILE * const fk = fopen(filename, "wb");
  if (fk == NULL) exit(1);

  sprintf(filename, "%s-vlen", argv[1]);
  FILE * const fv = fopen(filename, "wb");
  if (fv == NULL) exit(1);

  struct event e;
  uint64_t ts = 0;
  uint8_t k1; uint32_t k4;
  uint8_t v1; uint16_t v2; uint32_t v4;
  while (next_event(stdin, &ts, &e)) {

    if (e.klen) {
      k4 = e.klen;
      if (k4 < UINT64_C(0x100)) {
        k1 = (uint8_t)k4;
        fwrite(&k1, sizeof(k1), 1, fk);
      } else {
        fprintf(stderr, "error, klen > 255\n");
        exit(0);
      }
    }

    if (e.vlen) {
      v4 = e.vlen;
      if (v4 < UINT64_C(0x100)) {
        v1 = (uint8_t)v4;
        if (v1 == 1) v1 = 2; // 1 -> 2
        fwrite(&v1, sizeof(v1), 1, fv);
      } else if (v4 < UINT64_C(0x10000)) {
        v1 = 0;
        v2 = (uint16_t)v4;
        fwrite(&v1, sizeof(v1), 1, fv);
        fwrite(&v2, sizeof(v2), 1, fv);
      } else {
        v1 = 1;
        fwrite(&v1, sizeof(v1), 1, fv);
        fwrite(&v4, sizeof(v4), 1, fv);
      }
    }
  }
  fflush(fk); fclose(fk);
  fflush(fv); fclose(fv);
  return 0;
}
