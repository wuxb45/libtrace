/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Simplified trace format: 32-bit key
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
static bool __set_on_miss = true;
#include "lru.h"
#include "lirs.h"
#include "arc.h"

#define OUTSIDE ((UINT32_C(0xffffffff)))

// fixed value size for SLIDE paper ATC'17 Waldspurger

  void
runtrace(const char * const tracefile, const uint64_t max_cap,
    const struct rep_api * const api)
{
  static const uint32_t value_size = 4096 * 4;
  FILE * const ftrace = fopen(tracefile, "rb");
  assert(ftrace);
  void * const rep = api->op_new(0xfffffffeu, max_cap);

  uint32_t keys[65536];
  uint64_t nt = 0;
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 65536, ftrace);
    if (nkeys == 0) break;
    for (uint64_t i = 0; i < nkeys; i++) {
      api->op_set(rep, keys[i], value_size);
      nt++;
      if ((nt % UINT64_C(1000000)) == 0) {
        fprintf(stderr, "PID %6d W[%" PRIu64 "]\n", getpid(), nt);
        fflush(stderr);
      }
    }
  }
  const uint64_t ntrace = nt;
  const uint64_t nreport = nt>>8;
  nt = 0;
  rewind(ftrace);
  api->clean_stat(rep);
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 65536, ftrace);
    if (nkeys == 0) break;
    for (uint64_t i = 0; i < nkeys; i++) {
      api->op_get(rep, keys[i], value_size);
      nt++;
      if ((nt % nreport) == 0) {
        fprintf(stderr, "PID %6d R[%" PRIu64 "/%" PRIu64 "]\n", getpid(), nt, ntrace);
        fflush(stderr);
      }
    }
  }
  api->print(rep);
  fflush(stdout);
}

  int
main(int argc, char ** argv)
{
  if (argc != 4) {
    printf("usage: %s <algo> <trace> <max_cap>\n", argv[0]);
    exit(0);
  }

  const char * const tracefile = argv[2];
  const uint64_t max_cap = strtoull(argv[3], NULL, 10);

  struct rep_api * api = NULL;
  if (strcmp(argv[1], "lru") == 0) {
    api = &lru_api;
  } else if (strcmp(argv[1], "lirs") == 0) {
    api = &lirs_api;
  } else if (strcmp(argv[1], "arc") == 0) {
    api = &arc_api;
  } else {
    fprintf(stderr, "invalid api\n");
  }

  runtrace(tracefile, max_cap, api);
  exit(0);
}
