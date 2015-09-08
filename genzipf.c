
#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "generator.h"

  int
main(int argc ,char ** argv)
{
  struct GenInfo * const gi = generator_new_zipfian(UINT64_C(0), UINT64_C(0xffffffff));
  uint32_t buf[0x1000];
  for (uint64_t i = 0; i < 0x20000; i++) {
    for (uint64_t j = 0; i < 0x1000; i++) {
      buf[j] = (uint32_t)gi->next(gi);
    }
    fwrite(buf, sizeof(buf[0]), 0x1000, stdout);
  }
  fflush(stdout);
  exit(0);
}
