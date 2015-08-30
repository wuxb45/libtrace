#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  struct event e;
  uint64_t ts = 0;
  uint64_t limit = UINT64_C(0xffffffffffffffff);
  if (argc == 2) {
    limit = strtoull(argv[1], NULL, 10);
  }
  while (next_event(stdin, &ts, &e) && limit--) {
    printf("%c %016" PRIx64 "@%16" PRIu64 " %4" PRIu16 " %6" PRIu32 "\n", e.op, e.hkey, e.ts, e.klen, e.vlen);
  }
  return 0;
}
