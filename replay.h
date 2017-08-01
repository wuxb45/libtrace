/*
 * Copyright (c) 2015  Wu, Xingbo <wuxb45@gmail.com>
 *
 * All rights reserved. No warranty, explicit or implicit, provided.
 */
// Simplified trace format: 2-bit op and 30-bit key
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

#define OUTSIDE ((UINT32_C(0xffffffff)))

struct rep_api {
  void * (* op_new)(const uint32_t, const uint64_t);
  void (*op_set)(void *, const uint32_t, const uint32_t);
  void (*op_get)(void *, const uint32_t, const uint32_t);
  void (*op_del)(void *, const uint32_t);
  void (*print)(void *);
  void (*clean_stat)(void *);
};
