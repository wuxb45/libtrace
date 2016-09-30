/*
 * Copyright (c) 2016  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Extracting the klen/vlen information from trace in the original format (Yuehai's).
// It generates six files each containing a unique unit size. klen * 3 and vlen * 3.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 2) {
    printf("usage: %s <out-prefix>\n", argv[0]);
    exit(0);
  }
  char filename[4096];

  sprintf(filename, "%s-k1.bin", argv[1]);
  FILE * const fk1 = fopen(filename, "wb");
  if (fk1 == NULL) exit(1);
  sprintf(filename, "%s-k2.bin", argv[1]);
  FILE * const fk2 = fopen(filename, "wb");
  if (fk2 == NULL) exit(1);
  sprintf(filename, "%s-k4.bin", argv[1]);
  FILE * const fk4 = fopen(filename, "wb");
  if (fk4 == NULL) exit(1);

  sprintf(filename, "%s-v1.bin", argv[1]);
  FILE * const fv1 = fopen(filename, "wb");
  if (fv1 == NULL) exit(1);
  sprintf(filename, "%s-v2.bin", argv[1]);
  FILE * const fv2 = fopen(filename, "wb");
  if (fv2 == NULL) exit(1);
  sprintf(filename, "%s-v4.bin", argv[1]);
  FILE * const fv4 = fopen(filename, "wb");
  if (fv4 == NULL) exit(1);

  uint64_t ck1 = 0; uint64_t ck2 = 0; uint64_t ck4 = 0;
  uint64_t cv1 = 0; uint64_t cv2 = 0; uint64_t cv4 = 0;

  struct event e;
  uint64_t ts = 0;
  uint8_t k1; uint16_t k2; uint32_t k4;
  uint8_t v1; uint16_t v2; uint32_t v4;
  while (next_event(stdin, &ts, &e)) {

    if (e.klen) {
      k4 = e.klen;
      if (k4 < UINT64_C(0x100)) {
        k1 = (uint8_t)k4;
        fwrite(&k1, sizeof(k1), 1, fk1);
        ck1++;
      } else if (k4 < UINT64_C(0x10000)) {
        k2 = (uint16_t)k4;
        fwrite(&k2, sizeof(k2), 1, fk2);
        ck2++;
      } else {
        fwrite(&k4, sizeof(k4), 1, fk4);
        ck4++;
      }
    }

    if (e.vlen) {
      v4 = e.vlen;
      if (v4 < UINT64_C(0x100)) {
        v1 = (uint8_t)v4;
        fwrite(&v1, sizeof(v1), 1, fv1);
        cv1++;
      } else if (v4 < UINT64_C(0x10000)) {
        v2 = (uint16_t)v4;
        fwrite(&v2, sizeof(v2), 1, fv2);
        cv2++;
      } else {
        fwrite(&v4, sizeof(v4), 1, fv4);
        cv4++;
      }
    }
  }
  fflush(fk1); fclose(fk1);
  fflush(fk2); fclose(fk2);
  fflush(fk4); fclose(fk4);
  fflush(fv1); fclose(fv1);
  fflush(fv2); fclose(fv2);
  fflush(fv4); fclose(fv4);

  fprintf(stdout, "k1 %" PRIu64 " k2 %" PRIu64 " k4 %" PRIu64 "\n", ck1, ck2, ck4);
  fprintf(stdout, "v1 %" PRIu64 " v2 %" PRIu64 " v4 %" PRIu64 "\n", cv1, cv2, cv4);
  fflush(stdout);
  return 0;
}
