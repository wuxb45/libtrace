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
  struct slot ** const slots = (typeof(slots))malloc(sizeof(*slots) * N1);
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = (typeof(slot))malloc(sizeof(*slot) + (8 * sizeof(slot->entries[0])));
    slot->alloc = 8;
    slot->nr = 0;
    slots[i] = slot;
  }
  struct event e;
  uint64_t ts =0;
  uint64_t get = 0;
  while(next_event(stdin, &ts, &e)) {
    if (e.op == 'G') {
      one_get(slots, e.hkey);
      get++;
    }
  }
  uint64_t xcount[1024] = {};
  for (uint64_t i = 0; i < N1; i++) {
    struct slot * const slot = slots[i];
    for (uint64_t j = 0; j < slot->nr; j++) {
      const uint64_t c = slot->entries[j].count;
      if (c < 1024) {
        xcount[c]++;
      } else {
        printf("%016" PRIx64 " %6" PRIu64 "\n", slot->entries[j].hkey, slot->entries[j].count);
      }
    }
  }
  for (uint64_t i = 0; i < 1024; i++) {
    if (xcount[i]) printf("X %04" PRIu64 " %" PRIu64 "\n", i, xcount[i]);
  }
  return 0;
}
