/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "replay.h"

struct lirs {
  uint32_t nr_keys;
  uint32_t resi_keys;
  uint32_t lru_keys;
  uint32_t hir_keys;

  uint64_t max_resi_cap;
  uint64_t cur_resi_cap;
  uint64_t max_hir_cap;
  uint64_t cur_hir_cap;

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
    uint32_t rprev;
    uint32_t rnext;
    uint32_t hprev;
    uint32_t hnext;
  } __attribute__((packed)) arr[];
};

  static void *
lirs_new(const uint32_t nr_keys, const uint64_t max_cap)
{
  const size_t sz = sizeof(struct lirs) + ((nr_keys + 1) * sizeof(((struct lirs *)NULL)->arr[0]));
  struct lirs * const lirs = (typeof(lirs))malloc(sz);
  assert(lirs);
  bzero(lirs, sz);
  lirs->nr_keys = nr_keys;
  lirs->max_resi_cap = max_cap;
  lirs->max_hir_cap = max_cap >> 7; // 1/128
  assert(lirs->max_hir_cap > 0);
  // root node
  for (uint64_t i = 0; i <= nr_keys; i++) {
    lirs->arr[i].prev = (i < nr_keys)?OUTSIDE:nr_keys;
    lirs->arr[i].next = (i < nr_keys)?OUTSIDE:nr_keys;
    lirs->arr[i].rprev = (i < nr_keys)?OUTSIDE:nr_keys;
    lirs->arr[i].rnext = (i < nr_keys)?OUTSIDE:nr_keys;
    lirs->arr[i].hprev = (i < nr_keys)?OUTSIDE:nr_keys;
    lirs->arr[i].hnext = (i < nr_keys)?OUTSIDE:nr_keys;
  }
  return (void *)lirs;
}

  static inline bool
lirs_resident(struct lirs * const lirs, const uint32_t key)
{
  assert(key < lirs->nr_keys);
  if (lirs->arr[key].rprev == OUTSIDE) {
    assert(lirs->arr[key].rnext == OUTSIDE);
    return false;
  } else {
    return true;
  }
}

  static inline bool
lirs_in_lru(struct lirs * const lirs, const uint32_t key)
{
  assert(key < lirs->nr_keys);
  if (lirs->arr[key].prev == OUTSIDE) {
    assert(lirs->arr[key].next == OUTSIDE);
    return false;
  } else {
    return true;
  }
}

  static inline bool
lirs_in_hir(struct lirs * const lirs, const uint32_t key)
{
  assert(key < lirs->nr_keys);
  if (lirs->arr[key].hprev == OUTSIDE) {
    assert(lirs->arr[key].hnext == OUTSIDE);
    return false;
  } else {
    return true;
  }
}

  static inline void
lirs_resi_remove(struct lirs * const lirs, const uint32_t key)
{
  assert(key < lirs->nr_keys);
  if (lirs_resident(lirs, key)) { // in here
    assert(lirs->resi_keys > 0);
    const uint32_t rprev = lirs->arr[key].rprev;
    const uint32_t rnext = lirs->arr[key].rnext;
    lirs->cur_resi_cap -= lirs->arr[key].size;
    lirs->arr[rprev].rnext = rnext;
    lirs->arr[rnext].rprev = rprev;
    // clean up
    lirs->arr[key].rprev = OUTSIDE;
    lirs->arr[key].rnext = OUTSIDE;
    lirs->arr[key].size = 0;
    lirs->resi_keys--;
  }
}

  static inline void
lirs_lru_remove(struct lirs * const lirs, const uint32_t key)
{
  assert(key < lirs->nr_keys);
  assert(false == lirs_in_hir(lirs, key));
  if (lirs_in_lru(lirs, key)) { // in here
    assert(lirs->lru_keys > 0);
    const uint32_t prev = lirs->arr[key].prev;
    const uint32_t next = lirs->arr[key].next;
    lirs_resi_remove(lirs, key);
    lirs->arr[prev].next = next;
    lirs->arr[next].prev = prev;
    // clean up
    lirs->arr[key].prev = OUTSIDE;
    lirs->arr[key].next = OUTSIDE;
    lirs->lru_keys--;
  }
}

  static inline void
lirs_hir_remove(struct lirs * const lirs, const uint32_t key)
{
  assert(lirs_resident(lirs, key));
  assert(lirs_in_hir(lirs, key));
  const uint32_t hnext = lirs->arr[key].hnext;
  const uint32_t hprev = lirs->arr[key].hprev;
  lirs->arr[hnext].hprev = hprev;
  lirs->arr[hprev].hnext = hnext;
  lirs->arr[key].hnext = OUTSIDE;
  lirs->arr[key].hprev = OUTSIDE;
  lirs->hir_keys--;
  lirs->cur_hir_cap -= lirs->arr[key].size;
}

  static inline void
lirs_resi_insert(struct lirs * const lirs, const uint32_t key)
{
  assert(false == lirs_resident(lirs, key));
  const uint32_t nr_keys = lirs->nr_keys;
  const uint32_t head0 = lirs->arr[nr_keys].rnext;
  lirs->cur_resi_cap += lirs->arr[key].size;
  lirs->arr[key].rnext = head0;
  lirs->arr[key].rprev = nr_keys;
  lirs->arr[head0].rprev = key;
  lirs->arr[nr_keys].rnext = key;
  lirs->resi_keys++;
}

  static inline void
lirs_lru_insert(struct lirs * const lirs, const uint32_t key, const uint32_t size)
{
  assert(false == lirs_in_lru(lirs, key));
  const uint32_t nr_keys = lirs->nr_keys;
  const uint32_t head0 = lirs->arr[nr_keys].next;
  lirs->arr[key].size = size;
  lirs->arr[key].next = head0;
  lirs->arr[key].prev = nr_keys;
  lirs->arr[head0].prev = key;
  lirs->arr[nr_keys].next = key;
  lirs->lru_keys++;
  assert(lirs->lru_keys <= nr_keys);
  lirs_resi_insert(lirs, key);
}

  static inline void
lirs_lru_cleanup(struct lirs * const lirs)
{
  const uint32_t nr_keys = lirs->nr_keys;
  for (;;) {
    const uint32_t last = lirs->arr[nr_keys].prev;
    if (false == lirs_resident(lirs, last)) {
      lirs_lru_remove(lirs, last);
    } else {
      break;
    }
  }
}

  static inline void
lirs_lru_evict(struct lirs * const lirs, const uint32_t key)
{
  assert(lirs_in_lru(lirs, key));
  assert(lirs_resident(lirs, key));
  lirs_resi_remove(lirs, key);
}

  static inline void
lirs_hir_insert(struct lirs * const lirs, const uint32_t key)
{
  assert(lirs_resident(lirs, key));
  assert(false == lirs_in_hir(lirs, key));
  const uint32_t nr_keys = lirs->nr_keys;
  const uint32_t head0 = lirs->arr[nr_keys].hnext;
  lirs->arr[key].hnext = head0;
  lirs->arr[key].hprev = nr_keys;
  lirs->arr[head0].hprev = key;
  lirs->arr[nr_keys].hnext = key;
  lirs->hir_keys++;
  lirs->cur_hir_cap += lirs->arr[key].size;
}

// always evict from hir
  static inline void
lirs_evict1(struct lirs * const lirs)
{
  const uint32_t nr_keys = lirs->nr_keys;
  const uint32_t victim = lirs->arr[nr_keys].hprev;
  assert(victim < nr_keys);
  assert(lirs_in_hir(lirs, victim));
  assert(lirs_in_lru(lirs, victim));
  assert(lirs_resident(lirs, victim));
  lirs_hir_remove(lirs, victim);
  lirs_lru_evict(lirs, victim);
  lirs_lru_cleanup(lirs);
}

  static void
lirs_eviction(struct lirs * const lirs)
{
  const uint32_t nr_keys = lirs->nr_keys;
  // clean lir
  // while (lir > limit)
  uint32_t victim = lirs->arr[nr_keys].rprev;
  while ((lirs->cur_resi_cap - lirs->cur_hir_cap) > (lirs->max_resi_cap - lirs->max_hir_cap)) {
    // move lir.last -> hir.head
    while (victim < nr_keys && lirs_in_hir(lirs, victim)) {
      victim = lirs->arr[victim].rprev;
    }
    assert(victim < nr_keys);
    assert(lirs_in_lru(lirs, victim));
    assert(false == lirs_in_hir(lirs, victim));
    lirs_hir_insert(lirs, victim);
  }
  // evict from hir
  while (lirs->cur_resi_cap > lirs->max_resi_cap) {
    lirs_evict1(lirs);
  }
}

  static void
lirs_set(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lirs * const lirs = (typeof(lirs))ptr;
  bool insert_hir = false;
  if (lirs_resident(lirs, key)) { // hit
    assert(lirs_in_lru(lirs, key));
    if (lirs_in_hir(lirs, key)) { // was in hir list
      lirs_hir_remove(lirs, key); //promote to lir
    }
  } else {
    if (false == lirs_in_lru(lirs, key)) {// deadly cold
      insert_hir = true;
    }
  }
  lirs_lru_remove(lirs, key);
  lirs_lru_insert(lirs, key, size);
  lirs_lru_cleanup(lirs);
  if (insert_hir) {
    lirs_hir_insert(lirs, key); // mark as hir
  }
  // eviction
  lirs_eviction(lirs);
}

  static void
lirs_get(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct lirs * const lirs = (typeof(lirs))ptr;
  if (lirs_resident(lirs, key)) {
    lirs->nr_hit++;
    const uint32_t size0 = lirs->arr[key].size;
    lirs_set(ptr, key, size0);
  } else {
    lirs->nr_mis++;
    if (__set_on_miss) {
      lirs_set(ptr, key, size);
    }
  }
}

  static void
lirs_del(void * const ptr, const uint32_t key)
{
  struct lirs * const lirs = (typeof(lirs))ptr;
  if (lirs_resident(lirs, key)) {
    if (lirs_in_hir(lirs, key)) {
      lirs_hir_remove(lirs, key);
    }
    lirs_lru_remove(lirs, key);
    lirs_lru_cleanup(lirs);
  }
}

  static void
lirs_print(void * const ptr)
{
  struct lirs * const lirs = (typeof(lirs))ptr;
  const double all = (double)(lirs->nr_hit + lirs->nr_mis);
  const double hr = (double)(lirs->nr_hit);
  printf("lirs max %016" PRIu64 " cur %016" PRIu64 " hit %016" PRIu64 " mis %016" PRIu64 " hitratio %.6lf\n",
      lirs->max_resi_cap, lirs->cur_resi_cap, lirs->nr_hit, lirs->nr_mis, hr/all);
  fflush(stdout);
}

  static void
lirs_clean_stat(void * const ptr)
{
  struct lirs * const lirs = (typeof(lirs))ptr;
  lirs->nr_hit = 0;
  lirs->nr_mis = 0;
}

static struct rep_api lirs_api = {
  .op_new = lirs_new,
  .op_set = lirs_set,
  .op_get = lirs_get,
  .op_del = lirs_del,
  .print = lirs_print,
  .clean_stat = lirs_clean_stat,
};
