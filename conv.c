#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "libtrace.h"

struct slot {
  uint64_t alloc;
  uint64_t nr;
  struct {
    uint64_t hkey;
    uint64_t id;
  } entries[];
};

static uint64_t next_id = 0;

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

struct out {
  uint32_t id:30;
  uint32_t op:2;
} __attribute__((packed));

  void
one_op(struct slot ** const slots, const uint64_t hkey, uint32_t op)
{
  struct slot * slot = slots[hkey % N1];
  struct out o;
  o.op = op;
  for (uint64_t i = 0; i < slot->nr; i++) {
    if (slot->entries[i].hkey == hkey) {
      o.id = slot->entries[i].id;
      write(1, &o, sizeof(o));
      return;
    }
  }
  // not found
  o.id = next_id;
  write(1, &o, sizeof(o));

  if (slot->nr == slot->alloc) {
    slot = (typeof(slot))realloc(slot, sizeof(*slot) + (slot->alloc * 2 * sizeof(slot->entries[0])));
    slots[hkey % N1] = slot;
    slot->alloc *= 2;
  }
  slot->entries[slot->nr].hkey = hkey;
  slot->entries[slot->nr].id = next_id;
  slot->nr++;
  next_id++;
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
  while(next_event(stdin, &ts, &e)) {
    one_op(slots, e.hkey, (uint32_t)((e.flags & OP_MAP) >> 4));
  }
  return 0;
}
