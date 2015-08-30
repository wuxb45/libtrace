#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 5) {
    printf("usage: %s <out1> <out2> <out4> <out8>\n", argv[0]);
    exit(0);
  }
  FILE * const out1 = fopen(argv[1], "wb"); assert(out1);
  FILE * const out2 = fopen(argv[2], "wb"); assert(out2);
  FILE * const out4 = fopen(argv[3], "wb"); assert(out4);
  FILE * const out8 = fopen(argv[4], "wb"); assert(out8);
  uint64_t c1 = 0;
  uint64_t c2 = 0;
  uint64_t c4 = 0;
  uint64_t c8 = 0;

  struct event e;
  uint64_t ts = 0;
  uint8_t v1;
  uint16_t v2;
  uint32_t v4;
  uint64_t vlen;
  while (next_event(stdin, &ts, &e)) {
    if (e.vlen == 0) continue;
    vlen = e.vlen;
    if (vlen < UINT64_C(0x100)) {
      v1 = vlen;
      fwrite(&v1, sizeof(v1), 1, out1);
      c1++;
    } else if (vlen < UINT64_C(0x10000)) {
      v2 = vlen;
      fwrite(&v2, sizeof(v2), 1, out2);
      c2++;
    } else if (vlen < UINT64_C(0x100000000)) {
      v4 = vlen;
      fwrite(&v4, sizeof(v4), 1, out4);
      c4++;
    } else {
      fwrite(&vlen, sizeof(vlen), 1, out8);
      c8++;
    }
  }
  fflush(out1);
  fflush(out2);
  fflush(out4);
  fflush(out8);
  fclose(out1);
  fclose(out2);
  fclose(out4);
  fclose(out8);
  fprintf(stdout, "c1 %" PRIu64 " c2 %" PRIu64 " c4 %" PRIu64 " c8 %" PRIu64 "\n", c1, c2, c4, c8);
  fflush(stdout);
  return 0;
}
