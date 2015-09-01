#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "libtrace2.h"

struct lru {
  uint32_t nr_keys;
  uint64_t max_cap;
  uint64_t cur_cap;
  // stat
  uint64_t nr_set;
  uint64_t nr_add;
  uint64_t nr_get;
  uint64_t nr_del;
  // internal
  uint64_t nr_rmv; // really delete
  uint64_t nr_hit; // get-hit
  uint64_t nr_mis; // get-miss
  uint64_t nr_evi; // eviction

  struct {
    uint32_t size;
    uint32_t prev;
    uint32_t next;
  } arr[];
};

  static void *
lru_new(const uint32_t nr_keys, const uint64_t max_cap)
{
  const size_t sz = sizeof(struct lru) + ((nr_keys + 1) * sizeof(((struct lru *)NULL)->arr[0]));
  struct lru * const lru = (typeof(lru))malloc(sz);
  assert(lru);
  bzero(lru, sz);
  lru->nr_keys = nr_keys;
  lru->max_cap = max_cap;
  lru->cur_cap = 0;
  // root node
  lru->arr[nr_keys].size = 0;
  lru->arr[nr_keys].prev = nr_keys;
  lru->arr[nr_keys].next = nr_keys;
  return (void *)lru;
}

  static void
lru_set(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_set++;
  const uint32_t prev = lru->arr[key].prev;
  const uint32_t next = lru->arr[key].next;
  // remove if any
  if (prev < nr_keys || next < nr_keys) { // in here
    lru->cur_cap -= lru->arr[key].size;
    lru->arr[prev].next = next;
    lru->arr[next].prev = prev;
  }
  // insert
  lru->arr[key].size = size;
  const uint32_t head0 = lru->arr[nr_keys].next;
  lru->arr[key].next = head0;
  lru->arr[key].prev = nr_keys;
  lru->arr[head0].prev = key;
  lru->arr[nr_keys].next = key;
  lru->cur_cap += size;
  // eviction
  while (lru->cur_cap > lru->max_cap) {
    const uint32_t tail0 = lru->arr[nr_keys].prev;
    const uint32_t tail1 = lru->arr[tail0].prev;
    lru->arr[tail1].next = nr_keys;
    lru->arr[nr_keys].prev = tail1;
    lru->arr[tail0].prev = nr_keys;
    lru->cur_cap -= lru->arr[tail0].size;
    lru->nr_evi++;
  }
}

  static void
lru_get(void * const ptr, const uint32_t key)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_get++;
  const uint32_t prev = lru->arr[key].prev;
  const uint32_t next = lru->arr[key].next;
  if (prev < nr_keys || next < nr_keys) {
    lru->nr_hit++;
    lru->arr[prev].next = next;
    lru->arr[next].prev = prev;
    const uint32_t head0 = lru->arr[nr_keys].next;
    lru->arr[key].next = head0;
    lru->arr[key].prev = nr_keys;
    lru->arr[head0].prev = key;
    lru->arr[nr_keys].next = key;
  } else {
    lru->nr_mis++;
  }
}

  static void
lru_del(void * const ptr, const uint32_t key)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_del++;
  const uint32_t prev = lru->arr[key].prev;
  const uint32_t next = lru->arr[key].next;
  if (prev < nr_keys || next < nr_keys) {
    lru->nr_rmv++;
    lru->arr[prev].next = next;
    lru->arr[next].prev = prev;
    lru->arr[key].next = nr_keys;
    lru->arr[key].prev = nr_keys;
    lru->cur_cap -= lru->arr[key].size;
  }
}

  static void
lru_print(void * const ptr)
{
  struct lru * const lru = (typeof(lru))ptr;
  printf("[LRU] nr_keys %" PRIu32 " max_cap %" PRIu64 " cur_cap %" PRIu64 "\n", lru->nr_keys, lru->max_cap, lru->cur_cap);
  printf("[stat] set %" PRIu64 " get %" PRIu64 " del %" PRIu64 "\n", lru->nr_set, lru->nr_get, lru->nr_del);
  printf("[stat] rmv %" PRIu64 " hit %" PRIu64 " mis %" PRIu64 " evi %" PRIu64 "\n", lru->nr_rmv, lru->nr_hit, lru->nr_mis, lru->nr_evi);
  printf("[list]\n");
  const uint32_t nr_keys = lru->nr_keys;
  for (uint32_t id = lru->arr[nr_keys].next; id < nr_keys; id = lru->arr[id].next) {
    printf("%8" PRIu32 " %8" PRIu32 "\n", id, lru->arr[id].size);
  }
  printf("[list] end\n");
}

static struct rep_api lru_api = {
  .op_new = lru_new,
  .op_set = lru_set,
  .op_get = lru_get,
  .op_del = lru_del,
  .op_print = lru_print,
};
