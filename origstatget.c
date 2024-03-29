/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Statistics on the GET events of the trace in the original format (Yuehai's).

#include <stdint.h>
#include <stdlib.h>
#include "libtrace.h"

struct slot {
  uint64_t alloc;
  uint64_t nr;
  struct {
    uint64_t hkey;
    uint64_t count;
  } entries[];
};

#define N1 ((1024*1024*64))
#define NX ((8192))

// big->little
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

  void
one_get(struct slot ** const slots, const uint64_t hkey)
{
  struct slot * slot = slots[hkey % N1];
  for (uint64_t i = 0; i < slot->nr; i++) {
    if (slot->entries[i].hkey == hkey) {
      slot->entries[i].count++;
      return;
    }
  }
  // hot found
  if (slot->nr == slot->alloc) {
    slot = (typeof(slot))realloc(slot, sizeof(*slot) + (slot->alloc * 2 * sizeof(slot->entries[0])));
    slots[hkey % N1] = slot;
    slot->alloc *= 2;
  }
  slot->entries[slot->nr].hkey = hkey;
  slot->entries[slot->nr].count = 1;
  slot->nr++;
}

int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  struct slot ** const slots = (typeof(slots))malloc(sizeof(*slots) * N1);
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = (typeof(slot))malloc(sizeof(*slot) + (8 * sizeof(slot->entries[0])));
    slot->alloc = 8;
    slot->nr = 0;
    slots[i] = slot;
  }
  struct event e;
  uint64_t ts = 0;
  uint64_t get = 0;
  while(next_event(stdin, &ts, &e)) {
    if (e.op == 'G') {
      one_get(slots, e.hkey);
      get++;
    }
  }
  const uint64_t pk = get/1000u;
  uint64_t xcount[NX] = {};
  uint64_t y = 0;
  uint64_t keycount = 0;
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = slots[i];
    for (uint64_t j = 0; j < slot->nr; j++) {
      const uint64_t c = slot->entries[j].count;
      keycount++;
      if (c < NX) {
        xcount[c]++;
      } else {
        y++;
      }
    }
  }
  uint64_t * const ycount = (typeof(ycount))malloc(sizeof(*ycount) * y);
  const uint64_t yy = y;
  y=0;
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = slots[i];
    for (uint64_t j = 0; j < slot->nr; j++) {
      const uint64_t c = slot->entries[j].count;
      if (c >= NX) {
        ycount[y] = c;
        y++;
      }
    }
  }
  assert(yy==y);
  qsort(ycount, yy, sizeof(ycount[0]), __comp);

  uint64_t acc0 = 0;
  uint64_t nacc = 0;
  uint64_t nkey = 0;
  printf("==== %" PRIu64 " keys, %" PRIu64 " gets\n", keycount, get);
  for (uint64_t i = 0; i < yy; i++) {
    nacc += ycount[i];
    nkey ++;
    if ((nacc - acc0) > pk) {
      printf("nkey %" PRIu64 " (%.2lf%%) nacc %" PRIu64 " (%.2lf%%) // current access %" PRIu64 "\n", nkey, ((double)nkey)/((double)keycount)*100.0, nacc, ((double)nacc)/((double)get)*100.0, ycount[i]);
      acc0 = nacc;
    }
  }
  for (uint64_t i = NX-1; i > 0; i--) {
    nacc += i * xcount[i];
    nkey += xcount[i];
    if (((nacc - acc0) > pk) || (i == 1)) {
      printf("nkey %" PRIu64 " (%.2lf%%) nacc %" PRIu64 " (%.2lf%%) // current class %" PRIu64 " keys %" PRIu64 "\n", nkey, ((double)nkey)/((double)keycount)*100.0, nacc, ((double)nacc)/((double)get)*100.0, i, xcount[i]);
      acc0 = nacc;
    }
  }
  return 0;
}
