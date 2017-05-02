/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Statistics on the zipfian distribution.
// This program does not require any input data. The keys will be generated on-the-fly.
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/mman.h>

#include "generator.h"

#define MAXLOW (UINT64_C(0x180000000))
#define LOWMARK (UINT64_C(0x100000))
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
main(int argc ,char ** argv)
{
  (void)argc;
  (void)argv;
  uint64_t * const highs =   (typeof(highs))mmap(NULL, LOWMARK * sizeof(highs[0]), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  uint64_t * const lows =     (typeof(lows))mmap(NULL, MAXLOW * sizeof(lows[0]), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  uint64_t * const counts = (typeof(counts))mmap(NULL, MAXLOW * sizeof(counts[0]), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  assert(highs != MAP_FAILED);
  assert(lows != MAP_FAILED);
  assert(counts != MAP_FAILED);
  uint64_t nacc = 0;
  uint64_t lowid = 0;
  struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(1), UINT64_C(0x100000000000));
  while (lowid < MAXLOW) {
    const uint64_t x = gikey->next(gikey);
    if (x < LOWMARK) { highs[x]++; }
    else { lows[lowid] = x; lowid++; }
    nacc++;
  } // fill up lows[]
  //
  uint64_t keycount = 0;
  for (uint64_t i = 0; i < LOWMARK; i++) {
    if (highs[i]) {
      counts[keycount] = highs[i];
      keycount++;
    }
  }
  qsort(lows, MAXLOW, sizeof(lows[0]), __comp);
  uint64_t last = lows[0];
  uint64_t xcount = 1;
  for (uint64_t i = 1; i < MAXLOW; i++) {
    if (lows[i] != last) {
      counts[keycount] = xcount;
      keycount++;
      last = lows[i];
      xcount = 0;
    }
    xcount++;
  }
  counts[keycount] = xcount;
  keycount++;
  qsort(counts, keycount, sizeof(counts[0]), __comp);
  // get keycount
  //
  printf("== keycount %" PRIu64 " nacc %" PRIu64 "\n", keycount, nacc);
  const double allkeyd = (double)keycount;
  const double allaccd = (double)nacc;
  double next = 0.00001;
  uint64_t nget = 0;
  for (uint64_t i = 0; i < keycount; i++) {
    const double pkeys = ((double)(i+1)) / allkeyd;
    nget += counts[i];
    const double paccs = ((double)nget)/allaccd;
    if (paccs >= next) {
      printf("%16" PRIu64 " %.6lf %16" PRIu64 " %16" PRIu64 " %.6lf\n", i+1, pkeys, counts[i], nget, paccs);
      next += 0.00001;
    }
  }
  exit(0);
}
