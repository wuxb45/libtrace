#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "libtrace3.h"

struct lru {
  uint32_t nr_keys;
  uint32_t cur_keys;
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
  void (*receiver)(void * ptr, const uint32_t, const uint32_t);
  void * receiver_ptr;
  uint64_t * bitmap;
  uint64_t nr_bm;
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
  lru->cur_keys = 0;
  lru->max_cap = max_cap;
  lru->cur_cap = 0;
  const uint64_t nr_bm = (nr_keys>>6) + 1;
  const uint64_t size_bm = nr_bm << 3;
  lru->bitmap = (typeof(lru->bitmap))malloc(nr_bm * sizeof(lru->bitmap[0]));
  bzero(lru->bitmap, size_bm);
  for (uint64_t i = 0; i <= nr_keys; i++) {
    lru->arr[i].size = 0;
    lru->arr[i].prev = nr_keys;
    lru->arr[i].next = nr_keys;
  }
  return (void *)lru;
}

  static inline bool
lru_exist(struct lru * const lru, const uint32_t key)
{
  const uint32_t nr_keys = lru->nr_keys;
  const uint32_t prev = lru->arr[key].prev;
  const uint32_t next = lru->arr[key].next;
  if ((prev < nr_keys) || (next < nr_keys)) { // in here
    return true;
  } else {
    return false;
  }
}

  static inline void
lru_remove(struct lru * const lru, const uint32_t key)
{
  const uint32_t nr_keys = lru->nr_keys;
  const uint32_t prev = lru->arr[key].prev;
  const uint32_t next = lru->arr[key].next;
  if ((prev < nr_keys) || (next < nr_keys)) { // in here
    assert(lru->cur_cap > lru->arr[key].size);
    assert(lru->cur_keys > 0);
    lru->cur_cap -= lru->arr[key].size;
    lru->cur_keys--;
    lru->arr[prev].next = next;
    lru->arr[next].prev = prev;
    // clean up
    lru->arr[key].prev = nr_keys;
    lru->arr[key].next = nr_keys;
    lru->arr[key].size = 0;
    lru->bitmap[key>>6] &= (~(UINT64_C(1) << (key & 0x3fu)));
  }
}

  static inline void
lru_insert(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(lru->arr[key].next == nr_keys);
  assert(lru->arr[key].prev == nr_keys);
  const uint32_t head0 = lru->arr[nr_keys].next;
  lru->arr[key].size = size;
  lru->arr[key].next = head0;
  //lru->arr[key].prev = nr_keys;
  lru->arr[head0].prev = key;
  lru->arr[nr_keys].next = key;
  lru->cur_cap += size;
  lru->cur_keys++;
  assert(lru->cur_keys <= nr_keys);
  lru->bitmap[key>>6] |= (UINT64_C(1) << (key & 0x3fu));
}

  static void
lru_evict1(struct lru * const lru)
{
  assert(lru->nr_keys > 0);
  const uint32_t nr_keys = lru->nr_keys;
  const uint32_t tail0 = lru->arr[nr_keys].prev;
  const uint32_t size0 = lru->arr[tail0].size;
  lru_remove(lru, tail0);
  if (lru->receiver) lru->receiver(lru->receiver_ptr, tail0, size0);
  lru->nr_evi++;
}

  static void
lru_set(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_set++;

  if (lru_exist(lru, key)) {
    lru_remove(lru, key);
  }
  lru_insert(lru, key, size);
  // eviction
  while (lru->cur_cap > lru->max_cap) {
    lru_evict1(lru);
  }
}

  static void
lru_get(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_get++;

  if (lru_exist(lru, key)) {
    lru->nr_hit++;
    const uint32_t size0 = lru->arr[key].size;
    lru_remove(lru, key);
    lru_insert(lru, key, size0);
  } else {
    lru->nr_mis++;
    if (__set_on_miss) {
      lru_set(ptr, key, size);
    }
  }
}

  static void
lru_del(void * const ptr, const uint32_t key)
{
  struct lru * const lru = (typeof(lru))ptr;
  const uint32_t nr_keys = lru->nr_keys;
  assert(key < nr_keys);
  lru->nr_del++;

  if (lru_exist(lru, key)) {
    lru->nr_rmv++;
    lru_remove(lru, key);
  }
}

  static void
lru_print(void * const ptr)
{
  struct lru * const lru = (typeof(lru))ptr;
  const double all = (double)(lru->nr_hit + lru->nr_mis);
  const double hr = (double)(lru->nr_hit);
  printf("lru  max %016" PRIu64 " cur %016" PRIu64 " hit %016" PRIu64 " mis %016" PRIu64 " hitratio %.6lf\n",
      lru->max_cap, lru->cur_cap, lru->nr_hit, lru->nr_mis, hr/all);
  fflush(stdout);
}

  static void
lru_clean_stat(void * const ptr)
{
  struct lru * const lru = (typeof(lru))ptr;
  lru->nr_set = 0;
  lru->nr_add = 0;
  lru->nr_get = 0;
  lru->nr_del = 0;

  lru->nr_rmv = 0;
  lru->nr_hit = 0;
  lru->nr_mis = 0;
  lru->nr_evi = 0;
}

static struct rep_api lru_api = {
  .op_new = lru_new,
  .op_set = lru_set,
  .op_get = lru_get,
  .op_del = lru_del,
  .print = lru_print,
  .clean_stat = lru_clean_stat,
};
