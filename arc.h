#pragma once

#include <stdint.h>
#include <stdlib.h>
#include "libtrace3.h"

#define ARC_T1 ((0))
#define ARC_B1 ((1))
#define ARC_T2 ((2))
#define ARC_B2 ((3))

struct arc {
  uint32_t nr_keys;
  uint64_t max_cap;

  uint64_t p;

  //uint32_t nkeys[4];
  uint64_t caps[4];
  
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
    struct {
      uint32_t prev;
      uint32_t next;
    } node[4];
  } __attribute__((packed)) arr[];
};

  static void *
arc_new(const uint32_t nr_keys, const uint64_t max_cap)
{
  const size_t sz = sizeof(struct arc) + ((nr_keys + 1) * sizeof(((struct arc *)NULL)->arr[0]));
  struct arc * const arc = (typeof(arc))malloc(sz);
  assert(arc);
  bzero(arc, sz);
  arc->nr_keys = nr_keys;
  arc->max_cap = max_cap;
  for (uint64_t i = 0; i <= nr_keys; i++) {
    for (uint64_t j = 0; j <= 4; j++) {
      arc->arr[i].node[j].prev = nr_keys;
      arc->arr[i].node[j].next = nr_keys;
    }
  }
  return (void *)arc;
}

  static inline bool
arc_in(const uint32_t where, struct arc * const arc, const uint32_t key)
{
  const uint32_t nr_keys = arc->nr_keys;
  assert(where < 4);
  assert(key < nr_keys);
  return ((arc->arr[key].node[where].prev < nr_keys) || (arc->arr[key].node[where].next < nr_keys)) ? true : false;
}

  static inline void
arc_remove(const uint32_t where, struct arc * const arc, const uint32_t key)
{
  const uint32_t nr_keys = arc->nr_keys;
  assert(where < 4);
  if (arc_in(where, arc, key)) {
    const uint32_t next = arc->arr[key].node[where].next;
    const uint32_t prev = arc->arr[key].node[where].prev;
    arc->arr[next].node[where].prev = prev;
    arc->arr[prev].node[where].next = next;
    arc->arr[key].node[where].next = nr_keys;
    arc->arr[key].node[where].prev = nr_keys;
    // clean
    arc->caps[where] -= arc->arr[key].size;
    arc->arr[key].size = 0;
  }
}

  static inline void
arc_insert(const uint32_t where, struct arc * const arc, const uint32_t key, const uint32_t size)
{
  const uint32_t nr_keys = arc->nr_keys;
  assert(where < 4);
  assert(false == arc_in(where, arc, key));
  const uint32_t head0 = arc->arr[nr_keys].node[where].next;

  arc->arr[key].node[where].prev = nr_keys;
  arc->arr[key].node[where].next = head0;
  arc->arr[head0].node[where].prev = key;
  arc->arr[nr_keys].node[where].next = key;
  arc->arr[key].size = size;
  arc->caps[where] += size;
}

  static inline uint32_t
arc_lru(const uint32_t where, struct arc * const arc)
{
  const uint32_t nr_keys = arc->nr_keys;
  const uint32_t lru = arc->arr[nr_keys].node[where].prev;
  assert(lru < nr_keys);
  return lru;
}

  static inline void
arc_remove_lru(const uint32_t where, struct arc * const arc)
{
  const uint32_t nr_keys = arc->nr_keys;
  const uint32_t victim = arc->arr[nr_keys].node[where].prev;
  arc_remove(where, arc, victim);
}

  static inline bool
arc_resident(struct arc * const arc, const uint32_t key)
{
  return (arc_in(ARC_T1, arc, key) || arc_in(ARC_T2, arc, key)) ? true : false;
}

  static inline void
arc_move(const uint32_t fromwhere, const uint32_t towhere, struct arc * const arc, const uint32_t key)
{
  assert(arc_in(fromwhere, arc, key));
  assert(key < arc->nr_keys);
  const uint32_t size0 = arc->arr[key].size;
  arc_remove(fromwhere, arc, key);
  arc_insert(towhere, arc, key, size0);
}

  static inline void
arc_replace(struct arc * const arc, const uint32_t key)
{
  (void)key;
  if (arc->caps[ARC_T1] && (arc->caps[ARC_T1] >= arc->p)) {
    while (arc->caps[ARC_T1] >= arc->p) {
      const uint32_t victim = arc_lru(ARC_T1, arc);
      arc_move(ARC_T1, ARC_B1, arc, victim);
    }
  } else if (arc->caps[ARC_T2]) {
    const uint32_t victim = arc_lru(ARC_T2, arc);
    arc_move(ARC_T2, ARC_B2, arc, victim);
  }
}

  static inline void
arc_set(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct arc * const arc = (typeof(arc))ptr;
  if (arc_in(ARC_T1, arc, key)) { // case I.1
    arc_remove(ARC_T1, arc, key);
    arc_insert(ARC_T2, arc, key, size);

  } else if (arc_in(ARC_T2, arc, key)) { // Case I.1
    arc_remove(ARC_T2, arc, key);
    arc_insert(ARC_T2, arc, key, size);

  } else if (arc_in(ARC_B1, arc, key)) { // Case II
    const uint64_t d1 = (arc->caps[ARC_B1] > arc->caps[ARC_B2])?1:(arc->caps[ARC_B2]/arc->caps[ARC_B1]);
    const uint64_t pp = arc->p + d1;
    arc->p = (pp < arc->max_cap) ? pp : arc->max_cap;
    arc_replace(arc, key);
    arc_remove(ARC_B1, arc, key);
    arc_insert(ARC_T2, arc, key, size);

  } else if (arc_in(ARC_B2, arc, key)) { // Case III
    const uint64_t d2 = (arc->caps[ARC_B2] > arc->caps[ARC_B1])?1:(arc->caps[ARC_B1]/arc->caps[ARC_B2]);
    const uint64_t pp = arc->p - d2;
    arc->p = (arc->p < d2) ? 0 : pp;
    arc_replace(arc, key);
    arc_remove(ARC_B2, arc, key);
    arc_insert(ARC_T2, arc, key, size);
    
  } else { // Case IV
    while ((arc->caps[ARC_T1] + arc->caps[ARC_B1]) >= arc->max_cap) { // balance L1
      if (arc->caps[ARC_B1] != 0) {
        arc_remove_lru(ARC_B1, arc);
        arc_replace(arc, key);
      } else {
        arc_remove_lru(ARC_T1, arc);
      }
    }

    while (arc->caps[ARC_T1] + arc->caps[ARC_B1] + arc->caps[ARC_T2] + arc->caps[ARC_B2] >= arc->max_cap) { // Case IV.B
      if (arc->caps[ARC_T1] + arc->caps[ARC_B1] + arc->caps[ARC_T2] + arc->caps[ARC_B2] >= (arc->max_cap << 1)) {
        arc_remove_lru(ARC_B2, arc);
      }
      arc_replace(arc, key);
    }
    arc_insert(ARC_T1, arc, key, size);
  }
}

  static inline void
arc_get(void * const ptr, const uint32_t key, const uint32_t size)
{
  struct arc * const arc = (typeof(arc))ptr;
  if (arc_resident(arc, key)) {
    arc->nr_hit++;
    const uint64_t size0 = arc->arr[key].size;
    arc_set(ptr, key, size0);
  } else {
    arc->nr_mis++;
    if (__set_on_miss) {
      arc_set(ptr, key, size);
    }
  }
}

  static inline void
arc_del(void * const ptr, const uint32_t key)
{
  struct arc * const arc = (typeof(arc))ptr;
  arc_remove(ARC_T1, arc, key);
  arc_remove(ARC_T2, arc, key);

}
  static void
arc_print(void * const ptr)
{
  struct arc * const arc = (typeof(arc))ptr;
  const double all = (double)(arc->nr_hit + arc->nr_mis);
  const double hr = (double)(arc->nr_hit);
  const uint64_t cur_cap = arc->caps[ARC_T1] + arc->caps[ARC_T2];
  printf("arc  max %016" PRIu64 " cur %016" PRIu64 " hit %016" PRIu64 " mis %016" PRIu64 " hitratio %.6lf\n",
      arc->max_cap, cur_cap, arc->nr_hit, arc->nr_mis, hr/all);
  fflush(stdout);
}

  static void
arc_clean_stat(void * const ptr)
{
  struct arc * const arc = (typeof(arc))ptr;
  arc->nr_hit = 0;
  arc->nr_mis = 0;
}

static struct rep_api arc_api = {
  .op_new = arc_new,
  .op_set = arc_set,
  .op_get = arc_get,
  .op_del = arc_del,
  .print = arc_print,
  .clean_stat = arc_clean_stat,
};
