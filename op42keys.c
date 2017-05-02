/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Convert trace in op4 format into raw (hashed-)keys by removing opcodes
// Input: op4 trace
// Output: hash-key trace (u32)
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

  int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  uint64_t ops = 0;
  uint32_t keys[32];
  for (;;) {
    if (fread(&ops, sizeof(ops), 1, stdin) != 1) break;
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, stdin);
    fwrite(keys, sizeof(keys[0]), nkeys, stdout);
  }
  fflush(stderr);
  fflush(stdout);
  return 0;
}
