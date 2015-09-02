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

  static int 
__comp(const void * const p1, const void * const p2) 
{
  const uint32_t v1 = *((typeof(&v1))p1);
  const uint32_t v2 = *((typeof(&v2))p2);
  if (v1 < v2) {
    return -1; 
  } else if (v1 > v2) {
    return 1;
  } else {
    return 0;
  }
}

int
main(int argc, char ** argv)
{
  if (argc != 5) {
    printf("usage: %s <vlen1> <vlen2> <vlen4> <output>\n", argv[0]);
    exit(0);
  }
  const int fd1 = open(argv[1], O_RDONLY);
  assert(fd1 >= 0);
  const int fd2 = open(argv[2], O_RDONLY);
  assert(fd2 >= 0);
  const int fd4 = open(argv[3], O_RDONLY);
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
  qsort(vlens, nr_vlen, sizeof(vlens[0]), __comp);
  const uint64_t inc = nr_vlen >> 16;
  
  uint32_t * const sps = (typeof(sps))malloc(65536 * sizeof(sps[0]));
  for (uint64_t i = 0; i < 65536; i++) {
    sps[i] = vlens[i*inc];
  }
  FILE * const fout = fopen(argv[4], "wb");
  fwrite(sps, sizeof(sps[0]), 65536, fout);
  fflush(fout);
  fclose(fout);
  return 0;
}
