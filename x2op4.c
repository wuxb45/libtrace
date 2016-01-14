/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Convert trace of samplex (2-30) format into trace of op4 (64-bit ops + 32 keys) format.

#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "libtrace3.h"

  int
main(int argc ,char ** argv)
{
  struct samplex xs[32];
  for (;;) {
    const size_t nr = fread(xs, sizeof(xs[0]), 32, stdin);
    if (nr == 0) break;
    uint64_t ops = 0;
    uint32_t keys[32] = {};
    for (size_t i = 0; i < nr; i++) {
      ops |= (((uint64_t)(xs[i].op)) << (i << 1));
      keys[i] = xs[i].keyx;
    }
    fwrite(&ops, sizeof(ops), 1, stdout);
    fwrite(keys, sizeof(keys[0]), nr, stdout);
  }
  fflush(stdout);
  exit(0);
}
