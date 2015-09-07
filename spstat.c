#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "libtrace3.h"

struct spcount {
  uint32_t key;
  uint32_t count;
};

static int
spcomp(const void * const p1, const void * const p2)
{
  const struct spcount * const sp1 = (typeof(sp1))p1;
  const struct spcount * const sp2 = (typeof(sp2))p2;
  if (sp1->count > sp2->count) {
    return -1;
  } else if (sp1->count < sp2->count) {
    return 1;
  } else {
    return 0;
  }
}

int main(int argc, char ** argv)
{
  if (argc != 4) {
    printf("usage: %s <trace> <nr_keys> <vlen>\n", argv[0]);
    exit(0);
  }
  const uint64_t nr_keys = strtoull(argv[2], NULL, 10);
  struct spcount * const cnt = (typeof(cnt))malloc(nr_keys * sizeof(cnt[0]));
  for (uint64_t i = 0; i < nr_keys; i++) {
    cnt[i].key = i;
    cnt[i].count = 0;
  }

  FILE * const fin = fopen(argv[1], "rb");
  assert(fin);

  struct samplex sp[256];
  uint64_t ncount = 0;
  for(;;) {
    const uint32_t nr = fread(sp, sizeof(sp[0]), 256, fin);
    if (nr == 0) break;
    for (uint64_t i = 0; i < nr; i++) {
      const uint32_t key = sp[i].keyx;
      assert(key < nr_keys);
      cnt[key].count++;
      ncount++;
    }
  }
  qsort(cnt, nr_keys, sizeof(cnt[0]), spcomp);

  FILE * const fvlen = fopen(argv[3], "rb");
  uint64_t vlencnt = 0;
  uint64_t vlensum = 0;
  uint32_t vlen;
  while (fread(&vlen, 1, sizeof(vlen), fvlen)) {
    vlencnt++;
    vlensum+=vlen;
  }
  const double avglen = ((double)vlensum)/((double)vlencnt);

  const double nk = (double)nr_keys;
  const double nd = (double)ncount;
  uint64_t cur = 0;
  printf("==%" PRIu64 "\n", ncount);
  double next = 0.05;
  for (uint64_t i = 0; i < nr_keys; i++) {
    cur += cnt[i].count;
    const double curd = (double)cur;
    const double pa = curd/nd;
    if (pa > next) {
      const double curk = (double)i;
      const double pk = curk/nk;
      printf("%8" PRIu64 " +%12" PRIu32 " cur %12" PRIu64 " pk %.4lf pa %.4lf cap %.4lf\n", i, cnt[i].count, cur, pk, pa, curk * avglen);
      next += 0.05;
    }
  }
  fflush(stdout);
  exit(0);
}
