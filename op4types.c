/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Statistics on the trace in sp4 format (64-bit ops + 32 32-bit keys)
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtrace.h"

  int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  uint64_t ops = 0;
  uint32_t keys[32];

  uint64_t nr_get = 0;
  uint64_t nr_set = 0;
  uint64_t nr_add = 0;
  uint64_t nr_del = 0;

  while (fread(&ops, sizeof(ops), 1, stdin) == 1) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, stdin);
    for (uint64_t i = 0; i < nkeys; i++) {
      switch ((ops >> (i<<1)) & UINT64_C(0x3)) {
        case OP_GET: {nr_get++;} break;
        case OP_ADD: {nr_add++;} break;
        case OP_SET: {nr_set++;} break;
        case OP_DEL: {nr_del++;} break;
      }
    }
  }
  printf("GASD: %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", nr_get, nr_add, nr_set, nr_del);
  fflush(stdout);
  return 0;
}
