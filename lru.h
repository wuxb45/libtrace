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
  // root node
  for (uint64_t i = 0; i <= nr_keys; i++) {
    lru->arr[i].size = 0;
    lru->arr[i].prev = nr_keys;
    lru->arr[i].next = nr_keys;
  }
  return (void *)lru;
}

  static bool
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

  static void
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
  }
}

  static void
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
    lru_remove(lru, key);
    lru_insert(lru, key, size);
    while (lru->cur_cap > lru->max_cap) {
      lru_evict1(lru);
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
  //printf("max %" PRIu64 " cur %" PRIu64 " hit %" PRIu64 " mis %" PRIu64 " hitratio %.4lf\n", lru->max_cap, lru->cur_cap, lru->nr_hit, lru->nr_mis, hr/all);
  printf("lru  max %" PRIu64 " cur %" PRIu64 " hit %" PRIu64 " mis %" PRIu64 " hitratio %.4lf\n", lru->max_cap, lru->cur_cap, lru->nr_hit, lru->nr_mis, hr/all);
  /*
  printf("[LRU] nr_keys %" PRIu32 " cur_keys %" PRIu32 " max_cap %" PRIu64 " cur_cap %" PRIu64 "\n", lru->nr_keys, lru->cur_keys, lru->max_cap, lru->cur_cap);
  printf("[op] set %" PRIu64 " get %" PRIu64 " del %" PRIu64 "\n", lru->nr_set, lru->nr_get, lru->nr_del);
  printf("[event] rmv %" PRIu64 " hit %" PRIu64 " mis %" PRIu64 " evi %" PRIu64 "\n", lru->nr_rmv, lru->nr_hit, lru->nr_mis, lru->nr_evi);
  printf("[list]\n");
  const uint32_t nr_keys = lru->nr_keys;
  for (uint32_t id = lru->arr[nr_keys].next; id < nr_keys; id = lru->arr[id].next) {
    printf("%8" PRIu32 " %8" PRIu32 "\n", id, lru->arr[id].size);
  }
  printf("[list] end\n");
  */
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

struct lrux {
  struct lru * lru1;
  struct lru * lru2;
  uint64_t nr_hit; // get-hit
  uint64_t nr_mis; // get-miss
};

  static void *
lrux_new(const uint32_t nr_keys, const uint64_t max_cap)
{
  struct lrux * const lrux = (typeof(lrux))malloc(sizeof(*lrux));
  bzero(lrux, sizeof(*lrux));
  lrux->lru1 = lru_new(nr_keys, max_cap >> 1);
  lrux->lru2 = lru_new(nr_keys, max_cap >> 1);
  lrux->lru1->receiver = lru_insert;
  lrux->lru1->receiver_ptr = lrux->lru2;
  return (void *)lrux;
}

  static void
lru_evict2(struct lru * const lru)
{
  const uint32_t nr_keys = lru->nr_keys;
  for (;;) {
    const uint32_t victim = ((uint32_t)random()) % nr_keys;
    const uint32_t prev = lru->arr[victim].prev;
    const uint32_t next = lru->arr[victim].next;
    if ((prev < nr_keys) || (next < nr_keys)) { // in here
      const uint32_t size0 = lru->arr[victim].size;
      lru_remove(lru, victim);
      if (lru->receiver) lru->receiver(lru->receiver_ptr, victim, size0);
      lru->nr_evi++;
      return;
    }
  }
}

  static void
lrux_set(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lrux * const lrux = (typeof(lrux))ptr;
  if (lru_exist(lrux->lru1, key)) {
    lru_remove(lrux->lru1, key);
  }
  if (lru_exist(lrux->lru2, key)) {
    lru_remove(lrux->lru2, key);
  }
  lru_insert(lrux->lru1, key, size);
  while (lrux->lru1->cur_cap > lrux->lru1->max_cap) {
    lru_evict1(lrux->lru1);
  }
  while (lrux->lru2->cur_cap > lrux->lru2->max_cap) {
    lru_evict2(lrux->lru2);
  }
}

  static void
lrux_get(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lrux * const lrux = (typeof(lrux))ptr;
  if (lru_exist(lrux->lru1, key)) {
    lrux->nr_hit++;
    lrux_set(ptr, key, lrux->lru1->arr[key].size);
  } else if (lru_exist(lrux->lru2, key)) {
    lrux->nr_hit++;
    lrux_set(ptr, key, lrux->lru2->arr[key].size);
  } else {
    lrux->nr_mis++;
    lrux_set(ptr, key, size);
  }
}

  static void
lrux_del(void * const ptr, const uint32_t key)
{
  struct lrux * const lrux = (typeof(lrux))ptr;
  if (lru_exist(lrux->lru1, key)) {
    lru_remove(lrux->lru1, key);
  }
  if (lru_exist(lrux->lru2, key)) {
    lru_remove(lrux->lru2, key);
  }
}

  static void
lrux_print(void * const ptr)
{
  struct lrux * const lrux = (typeof(lrux))ptr;
  const double all = (double)(lrux->nr_hit + lrux->nr_mis);
  const double hr = (double)(lrux->nr_hit);
  printf("lrux max %" PRIu64 " cur %" PRIu64 " hit %" PRIu64 " mis %" PRIu64 " hitratio %.4lf\n",
      lrux->lru1->max_cap + lrux->lru2->max_cap, lrux->lru1->cur_cap + lrux->lru2->cur_cap, lrux->nr_hit, lrux->nr_mis, hr/all);
  fflush(stdout);
}

  static void
lrux_clean_stat(void * const ptr)
{
  struct lrux * const lrux = (typeof(lrux))ptr;
  lrux->nr_hit = 0;
  lrux->nr_mis = 0;
}

static struct rep_api lrux_api = {
  .op_new = lrux_new,
  .op_set = lrux_set,
  .op_get = lrux_get,
  .op_del = lrux_del,
  .print = lrux_print,
  .clean_stat = lrux_clean_stat,
};
