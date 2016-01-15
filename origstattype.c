/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Statistics on the trace in the original format.
// It only counts the (%) of each type of operation.
#include <stdint.h>
#include <stdlib.h>
#include "libtrace.h"

int
main(int argc, char ** argv)
{
  uint64_t get = 0;
  uint64_t set = 0;
  uint64_t add = 0;
  uint64_t del = 0;
  uint64_t any = 0;
  uint64_t ts = 0;
  struct event e;
  while(next_event(stdin, &ts, &e)) {
    switch (e.op) {
      case 'G': get++; break;
      case 'S': set++; break;
      case 'A': add++; break;
      case 'D': del++; break;
      default:  any++; break;
    }
  }
  const uint64_t sum = get+set+add+del+any;
  printf("get %" PRIu64 " set %" PRIu64 " add %" PRIu64 " del %" PRIu64 " any %" PRIu64 " sum %" PRIu64 "\n", get, set, add, del, any, sum);
  printf("get%% %lf\n", ((double)get)/((double)sum));
  printf("set%% %lf\n", ((double)set)/((double)sum));
  printf("add%% %lf\n", ((double)add)/((double)sum));
  printf("del%% %lf\n", ((double)del)/((double)sum));
  printf("any%% %lf\n", ((double)any)/((double)sum));
  return 0;
}
