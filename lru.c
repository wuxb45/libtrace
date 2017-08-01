/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Run lru replacement algorithm with 2-30 trace format.
#include <stdio.h>
#include <stdlib.h>
#include "lru.h"

  int
main(int argc, char ** argv)
{
  if (argc != 3) {
    printf("usage: %s <trace> <max_cap>\n", argv[0]);
    exit(0);
  }
  const uint64_t max_cap = strtoull(argv[2], NULL, 10);
  runtrace(argv[1], max_cap, &lru_api);
  exit(0);
}
