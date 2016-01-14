/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Generate a random vlen file
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

  int
main(int argc, char ** argv)
{
  uint32_t sz[65536];
  for (uint64_t i = 0; i < 65536; i++) {
    sz[i] = random() % 200;
  }
  fwrite(sz, 65536, sizeof(sz[0]), stdout);
  return 0;
}
