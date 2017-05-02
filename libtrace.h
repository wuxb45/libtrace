/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

#define OP_GET ((0))
#define OP_ADD ((1))
#define OP_SET ((2))
#define OP_DEL ((3))

#define PROT_TCP ((0))
#define PROT_UDP ((1))

#define PROT_MAP     ((0x80))
#define OP_MAP       ((0x70))
#define KEYSIZE_MAP  ((0x08))
#define DELTA_MAP    ((0x04))
#define DATASIZE_MAP ((0x03))

struct event {
  char op;
  uint8_t flags;
  uint16_t klen;
  uint32_t vlen;
  uint64_t ts;
  uint64_t hkey;
};

  bool
next_event(FILE * const input, uint64_t * const time_last, struct event * const out)
{
  uint8_t flags;
  char op;
  uint8_t t1;
  uint16_t t2;
  uint32_t t4;
  uint64_t delta;
  uint64_t hkey;
  int nread;

  if (fread(&flags, sizeof(flags), 1, input) == 0) return false;

  out->flags = flags;
  switch ((flags & OP_MAP) >> 4) {
    case OP_GET: op = 'G'; break;
    case OP_ADD: op = 'A'; break;
    case OP_SET: op = 'S'; break;
    case OP_DEL: op = 'D'; break;
    default: op = '?'; break;
  }
  out->op = op;

  // keySize
  if (flags & KEYSIZE_MAP) {
    nread = fread(&t4, sizeof(t4), 1, input);
  } else {
    nread = fread(&t1, sizeof(t1), 1, input);
    t4 = t1;
  }
  if (nread != 1) return false;
  assert(t4 < 65536);
  out->klen = t4;

  // delta
  if (flags & DELTA_MAP) {
    nread = fread(&delta, sizeof(delta), 1, input);
  } else {
    nread = fread(&t2, sizeof(t2), 1, input);
    delta = t2;
  }
  if (nread != 1) return false;

  // dataSize
  switch (flags & DATASIZE_MAP) {
    case 3:
      nread = fread(&t4, sizeof(t4), 1, input);
      break;
    case 2:
      {
        nread = fread(&t2, sizeof(t2), 1, input);
        t4 = t2;
      }
      break;
    case 1:
      {
        nread = fread(&t1, sizeof(t1), 1, input);
        t4 = t1;
      }
      break;
    case 0:
      t4 = 0;
      break;
  }
  if (nread != 1) return false;
  out->vlen = t4;
  const uint64_t ts = (*time_last) + delta;
  *time_last = ts;
  out->ts = ts;

  nread = fread(&hkey, sizeof(hkey), 1, input);
  if (nread != 1) return false;
  out->hkey = hkey;
  return true;
}
