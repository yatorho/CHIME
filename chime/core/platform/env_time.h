// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_ENV_TIME_H_
#define CHIME_CORE_PLATFORM_ENV_TIME_H_

#include <stdint.h>

namespace chime {
namespace platform {

class EnvTime {
 public:
  static constexpr uint64_t MICROS_TO_PICOS = 1000ULL * 1000ULL;
  static constexpr uint64_t MICROS_TO_NANOS = 1000ULL;
  static constexpr uint64_t MILLIS_TO_MICROS = 1000ULL;
  static constexpr uint64_t MILLIS_TO_NANOS = 1000ULL * 1000ULL;
  static constexpr uint64_t NANOS_TO_PICOS = 1000ULL;
  static constexpr uint64_t SECONDS_TO_MILLIS = 1000ULL;
  static constexpr uint64_t SECONDS_TO_MICROS = 1000ULL * 1000ULL;
  static constexpr uint64_t SECONDS_TO_NANOS = 1000ULL * 1000ULL * 1000ULL;

  EnvTime() = default;
  virtual ~EnvTime() = default;

  static uint64_t now_nanos();

  static uint64_t now_micros() { return now_nanos() / MICROS_TO_NANOS; }

  static uint64_t now_seconds() { return now_nanos() / SECONDS_TO_NANOS; }

  virtual uint64_t get_overridable_now_nanos() const { return now_nanos(); }

  virtual uint64_t get_overridable_now_micros() const {
    return get_overridable_now_nanos() / MICROS_TO_NANOS;
  }

  virtual uint64_t get_overridable_now_seconds() const {
    return get_overridable_now_nanos() / SECONDS_TO_NANOS;
  }
};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_ENV_TIME_H_
