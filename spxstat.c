/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Statistics on the trace in the samplex format (spx).
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libtrace3.h"

  static int
__comp(const void * const p1, const void * const p2)
{
  const uint64_t v1 = *((const uint64_t *)p1);
  const uint64_t v2 = *((const uint64_t *)p2);
  if (v1 > v2) {
    return -1;
  } else if (v1 < v2) {
    return 1;
  } else {
    return 0;
  }
}

  int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  struct samplex keys[1024];

  uint64_t nr_get = 0;
  uint64_t nr_set = 0;
  uint64_t nr_add = 0;
  uint64_t nr_del = 0;
  uint32_t max_key = 0;

  uint64_t * const cnt = (typeof(cnt))mmap(NULL, UINT64_C(1) << (32+3), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  assert(cnt);
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 1024, stdin);
    if (nkeys == 0) break;
    for (uint64_t i = 0; i < nkeys; i++) {
      switch (keys[i].op) {
        case OP_GET: {nr_get++; cnt[keys[i].keyx]++; } break;
        case OP_ADD: {nr_add++;} break;
        case OP_SET: {nr_set++;} break;
        case OP_DEL: {nr_del++;} break;
      }
      if (keys[i].keyx > max_key) max_key = keys[i].keyx;
    }
  }
  printf("GASD: %" PRIu64 " %" PRIu64 " %" PRIu64 " %" PRIu64 "\n", nr_get, nr_add, nr_set, nr_del);
  const uint32_t nr_keys = max_key+1;
  printf("KEYS: %" PRIu32 "\n", nr_keys);
  qsort(cnt, nr_keys, sizeof(cnt[0]), __comp);
  const double key_countd = (double)nr_keys;
  const double get_countd = (double)nr_get;
  uint64_t acc_get = 0;
  double next_paccs = 0.00001;
  for (uint32_t i = 0; i < nr_keys; i++) {
    const double keysd = (double)(i+1);
    acc_get += cnt[i];
    const double pkeys = keysd / key_countd;
    const double paccs = ((double)acc_get) / get_countd;
    if (paccs >= next_paccs) {
      // #key, pkeys, access, acc_access, p_acc_access
      printf("%8" PRIu32 " %.6lf %16" PRIu64 " %16" PRIu64 " %.6lf\n", i+1, pkeys, cnt[i], acc_get, paccs);
      next_paccs += 0.00001;
    }
  }
  fflush(stdout);
  return 0;
}
