#include <stdio.h>
#include <stdlib.h>

#include "arc.h"

  int
main(int argc, char ** argv)
{
  if (argc != 5) {
    printf("usage: %s <trace> <vlen> <nr_keys> <max_cap>\n", argv[0]);
    exit(0);
  }
  const uint32_t nr_keys = strtoull(argv[3], NULL, 10);
  const uint64_t max_cap = strtoull(argv[4], NULL, 10);
  runtrace(argv[1], argv[2], nr_keys, max_cap, &arc_api);
  exit(0);
}
