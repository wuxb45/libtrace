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

#include "lru.h"
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 7) {
    printf("usage: %s <trace.4> <size.1> <size.2> <size.4> <nr-keys> <max-cap>\n", argv[0]);
    exit(0);
  }
  const int fd1 = open(argv[2], O_RDONLY);
  assert(fd1 >= 0);
  const int fd2 = open(argv[3], O_RDONLY);
  assert(fd2 >= 0);
  const int fd4 = open(argv[4], O_RDONLY);
  assert(fd4 >= 0);

  struct stat st;
  fstat(fd1, &st);
  const size_t s1 = st.st_size;
  const uint8_t * const map1 = (typeof(map1))mmap(NULL, s1, PROT_READ, MAP_PRIVATE, fd1, 0);
  fstat(fd2, &st);
  const size_t s2 = st.st_size;
  const uint16_t * const map2 = (typeof(map2))mmap(NULL, s2, PROT_READ, MAP_PRIVATE, fd2, 0);
  fstat(fd4, &st);
  const size_t s4 = st.st_size;
  const uint32_t * const map4 = (typeof(map4))mmap(NULL, s4, PROT_READ, MAP_PRIVATE, fd4, 0);
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
  
  FILE * const ftrace = fopen(argv[1], "rb");
  assert(ftrace);
  const uint64_t nr_keys = strtoull(argv[5], NULL, 10);
  const uint64_t max_cap = strtoull(argv[6], NULL, 10);
  struct lru * const lru = lru_new(nr_keys, max_cap);

  uint64_t ops = 0;
  uint32_t keys[32];
  for (;;) {
    if (fread(&ops, sizeof(ops), 1, stdin) != 1) break;
    const size_t nkeys = fread(keys, sizeof(keys[0]), 32, stdin);
    for (uint64_t i = 0; i < nkeys; i++) {
      char op; 
      switch ((ops >> (i<<1)) & UINT64_C(0x3)) {
        case OP_SET: case OP_ADD:
        {
          lru_set(lru, keys[i], vlens[random() % nr_vlen]);
        }
        break;
        case OP_GET:
        {
          lru_get(lru, keys[i]);
        }
        break;
        case OP_DEL:
        {
          lru_del(lru, keys[i]);
        }
        break;
      }   
    }   
  }
  lru_print(lru);
  exit(0);
}
