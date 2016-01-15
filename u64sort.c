/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Sort a file containing uint64_t[].

#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

  static int
__comp(const void * const p1, const void * const p2)
{
  const uint64_t v1 = *((const uint64_t *)p1);
  const uint64_t v2 = *((const uint64_t *)p2);
  if (v1 < v2) {
    return -1;
  } else if (v1 > v2) {
    return 1;
  } else {
    return 0;
  }
}

int
main(int argc, char ** argv)
{
  if (argc != 3) {
    printf("usage: %s <input> <output>\n", argv[0]);
    exit(0);
  }
  struct stat st;
  assert(sizeof(st.st_size) == 8);
  assert(sizeof(size_t) == 8);
  stat(argv[1], &st);
  const uint64_t size = st.st_size;
  assert((size % 8) == 0);
  const int fdinput = open(argv[1], O_RDONLY);
  assert(fdinput >= 0);
  void * const input = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fdinput, 0);
  assert(input != MAP_FAILED);
  const int fdoutput = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, 00644);
  ftruncate(fdoutput, size);
  fdatasync(fdoutput);
  void * const output = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fdoutput, 0);
  assert(output != MAP_FAILED);
  memcpy(output, input, size);
  munmap(input, size);
  close(fdinput);

  qsort(output, size>>3, sizeof(uint64_t), __comp);
  msync(output, size, MS_SYNC);
  munmap(output, size);
  fdatasync(fdoutput);
  close(fdoutput);
}
