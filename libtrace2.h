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

struct rep_api {
  void * (* op_new)(const uint32_t, const uint64_t);
  void (*op_set)(void *, const uint32_t, const uint32_t);
  void (*op_get)(void *, const uint32_t);
  void (*op_del)(void *, const uint32_t);
  void (*op_print)(void *);
};

  static void
runtrace(const char * const tracefile, const char * const sizefile1, const char * const sizefile2,
    const char * const sizefile4, const uint32_t nr_keys, const uint64_t max_cap, const struct rep_api * const api)
{
  const int fd1 = open(sizefile1, O_RDONLY);
  assert(fd1 >= 0);
  const int fd2 = open(sizefile2, O_RDONLY);
  assert(fd2 >= 0);
  const int fd4 = open(sizefile4, O_RDONLY);
  assert(fd4 >= 0);

  struct stat st;
  fstat(fd1, &st);
  const size_t s1 = st.st_size;
  uint8_t * const map1 = (typeof(map1))mmap(NULL, s1, PROT_READ, MAP_PRIVATE, fd1, 0);
  fstat(fd2, &st);
  const size_t s2 = st.st_size;
  uint16_t * const map2 = (typeof(map2))mmap(NULL, s2, PROT_READ, MAP_PRIVATE, fd2, 0);
  fstat(fd4, &st);
  const size_t s4 = st.st_size;
  uint32_t * const map4 = (typeof(map4))mmap(NULL, s4, PROT_READ, MAP_PRIVATE, fd4, 0);
  const size_t nr_vlen = s1 + (s2 >> 1) + (s4 >> 2);
  uint32_t * const vlens = (typeof(vlens))malloc(nr_vlen * sizeof(vlens[0]));
  size_t id = 0;
  for (uint64_t i = 0; i < s1; i++) {
    vlens[id++] = (typeof(vlens[0]))map1[i];
  }
  for (uint64_t i = 0; i < (s2 >> 1); i++) {
    vlens[id++] = (typeof(vlens[0]))map2[i];
  }
  for (uint64_t i = 0; i < (s4 >> 2); i++) {
    vlens[id++] = (typeof(vlens[0]))map4[i];
  }
  assert(id == nr_vlen);
  munmap(map1, s1);
  munmap(map2, s2);
  munmap(map4, s4);
  close(fd1);
  close(fd2);
  close(fd4);
  fprintf(stdout, "vlens read done\n");
  fflush(stdout);
  
  FILE * const ftrace = fopen(tracefile, "rb");
  assert(ftrace);
  void * const rep = api->op_new(nr_keys, max_cap);

  uint64_t ops = 0;
  uint32_t keys[32];
  for (;;) {
    if (fread(&ops, sizeof(ops), 1, ftrace) != 1) break;
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, ftrace);
    for (uint64_t i = 0; i < nkeys; i++) {
      char op; 
      switch ((ops >> (i<<1)) & UINT64_C(0x3)) {
        case OP_SET: case OP_ADD:
          api->op_set(rep, keys[i], vlens[random() % nr_vlen]);
          break;
        case OP_GET:
          api->op_get(rep, keys[i]);
          break;
        case OP_DEL:
          api->op_del(rep, keys[i]);
          break;
        default: break;
      }
    }   
  }
  api->op_print(rep);
}
