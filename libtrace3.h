#pragma once

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "libtrace.h"

struct samplex {
  uint32_t keyx:30;
  uint32_t op:2;
} __attribute__((packed));

struct rep_api {
  void * (* op_new)(const uint32_t, const uint64_t);
  void (*op_set)(void *, const uint32_t, const uint32_t);
  void (*op_get)(void *, const uint32_t, const uint32_t);
  void (*op_del)(void *, const uint32_t);
  void (*print)(void *);
  void (*clean_stat)(void *);
};

  static inline uint64_t
random64(void)
{
  // 62 bit random value;
  const uint64_t rand64 = (((uint64_t)random()) << 31) | ((uint64_t)random());
  return rand64;
}

static bool __set_on_miss = true;

  static void
runtrace(const char * const tracefile, const char * const sizefile,
    const uint32_t nr_keys, const uint64_t max_cap, const struct rep_api * const api)
{
  const int fdsize = open(sizefile, O_RDONLY);
  assert(fdsize >= 0);
  struct stat st;
  fstat(fdsize, &st);
  const size_t size = st.st_size;
  uint32_t * const mapsize = (typeof(mapsize))mmap(NULL, size, PROT_READ, MAP_PRIVATE, fdsize, 0);
  const size_t nr_vlen = (size >> 2);
  assert(nr_vlen == 65536);

  FILE * const ftrace = fopen(tracefile, "rb");
  assert(ftrace);
  void * const rep = api->op_new(nr_keys, max_cap);

  struct samplex keys[65536];
  uint64_t nt = 0;
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 65536, ftrace);
    if (nkeys == 0) break;
    for (uint64_t i = 0; i < nkeys; i++) {
      assert(keys[i].keyx < nr_keys);
      api->op_set(rep, keys[i].keyx, mapsize[random64() % nr_vlen]);
      nt++;
    }
  }
  const uint64_t ntrace = nt;
  const uint64_t nreport = nt>>8;
  nt = 0;
  rewind(ftrace);
  api->clean_stat(rep);
  for (;;) {
    const size_t nkeys = fread(keys, sizeof(keys[0]), 65536, ftrace);
    if (nkeys == 0) break;
    for (uint64_t i = 0; i < nkeys; i++) {
      assert(keys[i].keyx < nr_keys);
      switch (keys[i].op) {
        case OP_SET: case OP_ADD:
          api->op_set(rep, keys[i].keyx, mapsize[random64() % nr_vlen]);
          break;
        case OP_GET:
          api->op_get(rep, keys[i].keyx, mapsize[random64() % nr_vlen]);
          break;
        case OP_DEL:
          api->op_del(rep, keys[i].keyx);
          break;
        default: break;
      }
      nt++;
      if ((nt % nreport) == 0) {
        fprintf(stderr, "PID %6d [%" PRIu64 "/%" PRIu64 "]\n", getpid(), nt, ntrace);
        fflush(stderr);
      }
    }
  }
  api->print(rep);
  fflush(stdout);
}
