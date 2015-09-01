#include <stdio.h>
#include <stdlib.h>
#include "lru.h"

int
main(int argc, char ** argv)
{
  if (argc != 7) {
    printf("usage: %s <trace> <vlen1> <vlen2> <vlen4> <nr_keys> <max_cap>\n", argv[0]);
    exit(0);
  }
  const uint32_t nr_keys = strtoull(argv[5], NULL, 10);
  const uint64_t max_cap = strtoull(argv[6], NULL, 10);
  runtrace(argv[1], argv[2], argv[3], argv[4], nr_keys, max_cap, &lru_api);
}
