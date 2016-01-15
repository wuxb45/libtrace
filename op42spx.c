/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Convert trace in op4 format into samplex format by sampling only 1/(1<<shift) of the keys.
// args: <shift>
// Input: op4 trace
// Output: sp trace
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtrace.h"

struct samplex {
  uint32_t keyx:30;
  uint32_t op:2;
} __attribute__((packed));

  int
main(int argc, char ** argv)
{
  if (argc != 2) {
    printf("usage: %s <shift>\n", argv[0]);
    exit(0);
  }
  const uint64_t shift = strtoull(argv[1], NULL, 10);
  assert(shift < 32);
  const uint32_t mask = (1u << shift) - 1u;
  uint64_t ops = 0;
  uint32_t keys[32];
  struct samplex xarr[256];
  uint64_t xid = 0;
  uint32_t max_key = 0;
  for (;;) {
    if (fread(&ops, sizeof(ops), 1, stdin) != 1) break;
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, stdin);
    for (uint64_t i = 0; i < nkeys; i++) {
      if ((keys[i] & mask) == 0) {
        const uint32_t op = (ops >> (i<<1)) & UINT64_C(0x3);
        xarr[xid].op = op;
        xarr[xid].keyx = keys[i] >> shift;
        if (xarr[xid].keyx > max_key) max_key = xarr[xid].keyx;
        xid++;
        if (xid == 256) {
          fwrite(xarr, sizeof(xarr[0]), 256, stdout);
          xid=0;
        }
      }
    }
  }
  if (xid) {
    fwrite(xarr, sizeof(xarr[0]), xid, stdout);
  }
  fprintf(stderr, "max generated key %" PRIu32 "\n", max_key);
  fflush(stderr);
  fflush(stdout);
  return 0;
}
