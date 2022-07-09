// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/platform/env_time.h"

#include <chrono>

namespace chime {
namespace platform {

uint64_t EnvTime::now_nanos() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

}  // namespace platform
}  // namespace chime
