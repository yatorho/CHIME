// Copyright 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_OVERFLOW_H_
#define CHIME_CORE_UTIL_OVERFLOW_H_

#include "chime/core/platform/types.h"

namespace chime {

inline int64 MultiplyWithoutOverflow(const int64 x, const int64 y) {
  if (x < 0 || y < 0 ) return -1;
  if (x == 0 || y == 0) return 0;

  const uint64 ux = x;
  const uint64 uy = y;
  const uint64 uxy = ux * uy;

  if ((ux | uy) >> 32 != 0) {
    if (uxy / ux != uy) return -1;
  } 
  return static_cast<int64>(uxy); 
}

}  // namespace chime

#endif  // CHIME_CORE_UTIL_OVERFLOW_H_
