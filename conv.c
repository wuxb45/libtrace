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

struct lr {
  uint32_t l;
  uint32_t r;
};

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
  assert((size >> 3) < UINT64_C(0xffffffff));
  const int fdkeymap = open(argv[1], O_RDONLY);
  assert(fdkeymap >= 0);
  const uint64_t * const input = (const uint64_t *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fdkeymap, 0);
  assert(input != MAP_FAILED);
  const uint64_t right0 = (size>>3) - 1;

  struct lr *lr24 = (typeof(lr24))malloc(sizeof(*lr24) * 0x1000000);
  assert(lr24);
  {
    uint32_t id = 0;
    for (uint64_t head = 0; head < 0x1000000; head++) {
      while ((id <= right0) && ((input[id]>>40) < head)) {
        id++;
      }
      lr24[head].l = id;
    }
    for (uint64_t head = 0; head < 0xffffff; head++) {
      lr24[head].r = lr24[head+1].l-1;
    }
    lr24[0xffffff].r = right0;
  }

  struct event e;
  uint64_t ts = 0;
  while(next_event(stdin, &ts, &e)) {
    const uint64_t hkey = e.hkey;
    const uint64_t head = hkey >> 40;
    uint64_t li = lr24[head].l;
    uint64_t ri = lr24[head].r;
    uint64_t mid = 0;
    bool found = false;
    while (li <= ri) {
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
    if (!found) {
      fprintf(stderr, "hkey %" PRIx64 "\n", hkey);
      fflush(stderr);
      assert(false);
    }
    const uint32_t w = (typeof(w))mid;
    assert(((uint64_t)w) == mid);
    fwrite(&w, sizeof(w), 1, stdout);
  }
  munmap((void *)input, size);
  close(fdkeymap);
  fflush(stdout);
}
