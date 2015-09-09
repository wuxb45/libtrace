
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/mman.h>

#include "generator.h"

#define NRACC (UINT64_C(0x100000000))
#define MARK (UINT64_C(0x100000))
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
  uint64_t * const low = (typeof(low))mmap(NULL, UINT64_C(1) << (32+3), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  uint64_t lowid = 0;
  uint64_t * const cnt = (typeof(low))mmap(NULL, MARK <<3, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(1), UINT64_C(0x100000000000));
  for (uint64_t i = 0; i < NRACC; i++) {
    const uint64_t x = gikey->next(gikey);
    if (x < MARK) { cnt[x]++; }
    else { low[lowid] = x; lowid++; }
  }
  //
  uint64_t * const counts = (typeof(counts))mmap(NULL, UINT64_C(1) << (32+3), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_HUGETLB | MAP_ANONYMOUS, -1, 0);
  uint64_t keycount = 0;
  for (uint64_t i = 0; i < MARK; i++) {
    if (cnt[i]) {
      counts[keycount] = cnt[i];
      keycount++;
    }
  }
  qsort(low, lowid, sizeof(low[0]), __comp);
  uint64_t last = low[0];
  uint64_t xcount = 1;
  for (uint64_t i = 1; i < lowid; i++) {
    if (low[i] != last) {
      counts[keycount] = xcount;
      keycount++;
      last = low[i];
      xcount = 0;
    }
    xcount++;
  }
  counts[keycount] = xcount;
  keycount++;
  qsort(counts, keycount, sizeof(counts[0]), __comp);
  // get keycount
  //
  const double allkeyd = (double)keycount;
  const double allaccd = (double)NRACC;
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
