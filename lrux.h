#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "lru.h"

struct lrux {
  struct lru * lru1;
  struct lru * lru2;
  uint64_t nr_hit; // get-hit
  uint64_t nr_mis; // get-miss
};

static uint64_t lrux_lru_cap;
  static void *
lrux_new(const uint32_t nr_keys, const uint64_t max_cap)
{
  struct lrux * const lrux = (typeof(lrux))malloc(sizeof(*lrux));
  bzero(lrux, sizeof(*lrux));
  const uint64_t size1 = (lrux_lru_cap < max_cap) ? lrux_lru_cap : max_cap;
  const uint64_t size2 = (lrux_lru_cap < max_cap) ? (max_cap - lrux_lru_cap) : 0;
  assert((size1 + size2) == max_cap);
  lrux->lru1 = lru_new(nr_keys, size1);
  lrux->lru2 = lru_new(nr_keys, size2);
  lrux->lru1->receiver = lru_insert;
  lrux->lru1->receiver_ptr = lrux->lru2;
  return (void *)lrux;
}

  static void
lrux_evict2(struct lru * const lru)
{
  const uint32_t nr_keys = lru->nr_keys;
  if ((lru->cur_keys > 4096) && (lru->cur_keys > (nr_keys >> 10))) {
    const uint32_t seed = ((uint32_t)random()) % nr_keys;
    for (uint32_t i = 0; i < nr_keys; i++) {
      const uint32_t victim = (seed + i) % nr_keys;
      if ((lru->bitmap[victim>>6] & (UINT64_C(1) << (victim & 0x3fu))) == 0) continue;
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
  } else {
    const uint32_t skip = ((uint32_t)random()) % lru->cur_keys;
    uint32_t iter;
    if (skip < (lru->cur_keys >> 1)) {
      iter = lru->arr[nr_keys].next;
      assert(iter < nr_keys);
      for (uint32_t i = 0; i < skip; i++) {
        iter = lru->arr[iter].next;
        assert(iter < nr_keys);
      }
    } else {
      const uint32_t rskip = lru->cur_keys - skip;
      iter = lru->arr[nr_keys].prev;
      assert(iter < nr_keys);
      for (uint32_t i = 0; i < rskip; i++) {
        iter = lru->arr[iter].prev;
        assert(iter < nr_keys);
      }
    }
    const uint32_t victim = iter;
    const uint32_t size0 = lru->arr[victim].size;
    lru_remove(lru, victim);
    if (lru->receiver) lru->receiver(lru->receiver_ptr, victim, size0);
    lru->nr_evi++;
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
    lrux_evict2(lrux->lru2);
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
  printf("lrux max %16" PRIu64 " cur %16" PRIu64 " hit %16" PRIu64 " mis %16" PRIu64 " hitratio %.6lf\n",
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
