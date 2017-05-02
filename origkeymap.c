/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Generate a sorted keymap from the trace in original format (Yuehai's).
// Input: The original trace stream.
// Output: An array of uint64_t[], sorted.
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "libtrace.h"

struct slot {
  uint32_t alloc;
  uint32_t nr;
  uint64_t hkey[];
};

static uint64_t next_id = 0;

#define N1 ((1024*1024*64))
#define NX ((8192))

  void
one_key(struct slot ** const slots, const uint64_t hkey)
{
  struct slot * slot = slots[hkey % N1];
  for (uint64_t i = 0; i < slot->nr; i++) {
    if (slot->hkey[i] == hkey) {
      return;
    }
  }
  // not found
  if (slot->nr == slot->alloc) {
    slot = (typeof(slot))realloc(slot, sizeof(*slot) + ((slot->alloc + 4) * sizeof(slot->hkey[0])));
    slots[hkey % N1] = slot;
    slot->alloc += 4;
  }
  slot->hkey[slot->nr] = hkey;
  slot->nr++;
  next_id++;
}

int
main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;
  struct slot ** const slots = (typeof(slots))malloc(sizeof(*slots) * N1);
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = (typeof(slot))malloc(sizeof(*slot) + (8 * sizeof(slot->hkey[0])));
    slot->alloc = 8;
    slot->nr = 0;
    slots[i] = slot;
  }
  struct event e;
  uint64_t ts = 0;
  while(next_event(stdin, &ts, &e)) {
    one_key(slots, e.hkey);
  }
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = slots[i];
    fwrite(slot->hkey, sizeof(slot->hkey[0]), slot->nr, stdout);
  }
  fflush(stdout);
  return 0;
}
