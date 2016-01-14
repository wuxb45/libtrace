/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Print the trace stream in grouped ops and 32-bit keys (sp4) format.
// Input: A stream of op4 events.
// Output: Text.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtrace.h"

int
main(int argc, char ** argv)
{
  uint64_t limit = UINT64_C(0xffffffffffffffff);
  if (argc == 2) {
    limit = strtoull(argv[1], NULL, 10);
  }
  uint64_t ops = 0;
  uint32_t keys[32];
  for (;;) {
    if (fread(&ops, sizeof(ops), 1, stdin) != 1) break;
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, stdin);
    for (uint64_t i = 0; i < nkeys; i++) {
      if (limit == 0) exit(0);
      limit--;
      char op;
      switch ((ops >> (i<<1)) & UINT64_C(0x3)) {
        case OP_GET: op = 'G'; break;
        case OP_ADD: op = 'A'; break;
        case OP_SET: op = 'S'; break;
        case OP_DEL: op = 'D'; break;
      }
      printf("%c %08" PRIx32 "\n", op, keys[i]);
    }
  }
  return 0;
}
