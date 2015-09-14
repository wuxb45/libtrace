#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "libtrace3.h"
#include "generator.h"

  static int 
__comp(const void * const p1, const void * const p2) 
{
  const uint64_t v1 = *((const uint64_t *)p1);
  const uint64_t v2 = *((const uint64_t *)p2);
  if (v1 < v2) {
    return -1; 
  } else if (v1 > v2) {
    return 1;
  } else {
    return 0;
  }
}

  int
main(int argc ,char ** argv)
{
  if (argc != 3) {
    printf("usage : %s <range> <nr-op>\n", argv[0]);
    exit(0);
  }
  const uint64_t range = strtoull(argv[1], NULL, 10);
  // 1G * 8 = 8G
  const uint64_t trace_nr = strtoull(argv[2], NULL, 10);
  uint64_t * const t64 = (typeof(t64))malloc(sizeof(t64[0]) * (trace_nr + 1));
  uint64_t * const shadow = (typeof(shadow))malloc(sizeof(shadow[0]) * (trace_nr + 1));
  uint64_t * const keymap = (typeof(keymap))malloc(sizeof(keymap[0]) * (trace_nr + 1));
  struct samplex * const xs = (typeof(xs))malloc(sizeof(xs[0]) * (trace_nr + 1));
  assert(t64);
  //struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(1), UINT64_C(0x100000000000));
  struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(1), range);
  for (uint64_t i = 0; i < trace_nr; i++) {
    const uint64_t x = gikey->next(gikey);
    t64[i] = x;
  }
  memcpy(shadow, t64, sizeof(t64[0]) * trace_nr);
  qsort(shadow, trace_nr, sizeof(shadow[0]), __comp);
  shadow[trace_nr] = shadow[trace_nr-1]+1;
  uint64_t id1 = 0;
  uint64_t id2 = 0;
  uint64_t idw = 0;
  while (id1 < trace_nr) {
    while (shadow[id1] == shadow[id2]) id2++;
    assert(shadow[id1] < shadow[id2]);
    keymap[idw]=shadow[id1];
    idw++;
    id1 = id2;
  }
  const uint64_t nr_keys = idw;
  for (uint64_t i = 0; i < trace_nr; i++) {
    const uint64_t key0 = t64[i];
    uint64_t start = 0;
    uint64_t end = nr_keys -1;
    bool found = false;
    while (start <= end) {
      const uint64_t mid = (start + end) >> 1;
      if (keymap[mid] < key0) {
        start = mid+1;
      } else if (keymap[mid] > key0) {
        end = mid-1;
      } else {
        xs[i].keyx = mid;
        if (random_double() < 0.05) {xs[i].op = OP_SET; } else {xs[i].op = OP_GET; }
        found = true;
        break;
      }
    }
    assert(found);
  }
  const size_t nw = fwrite(xs, sizeof(xs[0]), trace_nr, stdout);
  assert(nw == trace_nr);
  fflush(stdout);
  exit(0);
}
