/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Run lrux replacement algorithm with 2-30 trace format.
#include <stdio.h>
#include <stdlib.h>
#include "lrux.h"

  int
main(int argc, char ** argv)
{
  if (argc != 6) {
    printf("usage: %s <trace> <vlen> <nr_keys> <max_cap> <lru_cap>\n", argv[0]);
    exit(0);
  }
  const uint32_t nr_keys = strtoull(argv[3], NULL, 10);
  const uint64_t max_cap = strtoull(argv[4], NULL, 10);
  const uint64_t lru_cap = strtoull(argv[5], NULL, 10);
  if (lru_cap >= max_cap) exit(0);
  lrux_lru_cap = lru_cap;
  runtrace(argv[1], argv[2], nr_keys, max_cap, &lrux_api);
  exit(0);
}
