/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Generate trace in op4 format.
// <range>: [1, <range>]
// <pset>: set%
// <nr-op>: number of events to be generated.
// Output: An array of struct samplex[]
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "libtrace.h"
#include "generator.h"

  int
main(int argc ,char ** argv)
{
  if (argc < 4) {
    printf("usage : %s <range> <pset> <nr-op>\n", argv[0]);
    exit(0);
  }
  const uint64_t range = strtoull(argv[1], NULL, 10);
  const uint64_t pset = strtoull(argv[2], NULL, 10);
  const uint64_t nr = strtoull(argv[3], NULL, 10);
  struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(1), range);
  struct GenInfo * const giop = generator_new_uniform(UINT64_C(1), 100000000);
  uint64_t op4;
  uint32_t keys[32];
  uint64_t count = 0;
  while (count < nr) {
    op4 = 0;
    for (uint64_t i = 0; i < 32; i++) {
      keys[i] = gikey->next(gikey);
      count++;
      if ((giop->next(giop) % 100) < pset) {
        op4 |= (((uint64_t)OP_SET) << (i << 1));
      }
    }
    (void)fwrite(&op4, sizeof(op4), 1, stdout);
    (void)fwrite(keys, sizeof(keys[0]), 32, stdout);
  }
  fflush(stdout);
  exit(0);
}
