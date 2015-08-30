#define _GNU_SOURCE
#define _LARGEFILE64_SOURCE

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "libtrace.h"

int
main(int argc, char ** argv)
{
  if (argc != 2) {
    printf("usage: %s <keymap> <input >output\n", argv[0]);
    exit(0);
  }
  struct stat st;
  stat(argv[1], &st);
  const uint64_t size = st.st_size;
  assert((size % 8) == 0);
  assert(size < UINT64_C(0xffffffff));
  const int fdkeymap = open(argv[1], O_RDONLY);
  assert(fdkeymap >= 0);
  const uint64_t * const input = (const uint64_t *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fdkeymap, 0);
  assert(input != MAP_FAILED);
  const uint64_t right0 = (size>>3) - 1;

  struct event e;
  uint64_t ts = 0;
  while(next_event(stdin, &ts, &e)) {
    const uint64_t hkey = e.hkey;
    uint32_t li = 0;
    uint32_t ri = right0;
    uint32_t mid = 0;
    bool found = false;
    while (li < ri) {
      mid = (li + ri) >> 1;
      if (hkey < input[mid]) {
        ri = mid-1;
      } else if (hkey > input[mid]) {
        li = mid+1;
      } else {
        found = true;
        break;
      }
    }
    assert(found);
    fwrite(&mid, sizeof(mid), 1, stdout);
  }
  munmap((void *)input, size);
  close(fdkeymap);
  fflush(stdout);
}
