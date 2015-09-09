
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "generator.h"
#include "libtrace3.h"

  int
main(int argc ,char ** argv)
{
  if (argc != 3) {
    printf("usage: %s <get-portion(0-100)> <set-portion(0-100)>\n", argv[0]);
    exit(0);
  }
  const uint64_t getp = strtoull(argv[1], NULL, 10);
  const uint64_t setp = strtoull(argv[2], NULL, 10);
  const uint64_t getsetp = getp + setp;
  assert(getsetp <= 100);

  struct GenInfo * const gikey = generator_new_zipfian(UINT64_C(0), UINT64_C(0x3fffffff));
  struct GenInfo * const giop = generator_new_uniform(UINT64_C(0), UINT64_C(99));
  struct samplex buf[0x1000];
  for (uint64_t i = 0; i < 0x20000; i++) {
    for (uint64_t j = 0; i < 0x1000; i++) {
      buf[j].keyx = (uint32_t)gikey->next(gikey);
      const uint64_t opp = giop->next(giop);
      if (opp < getp) {
        buf[j].op = OP_GET;
      } else if (opp < getsetp) {
        buf[j].op = OP_SET;
      } else {
        buf[j].op = OP_DEL;
      }
    }
    fwrite(buf, sizeof(buf[0]), 0x1000, stdout);
  }
  fflush(stdout);
  exit(0);
}
