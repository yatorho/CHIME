/* Copyright by 2022 The Chime Developers. All rights reserved.
 * 
 * License under Apache License, Version 2.0.
 * See the LICENSE file at the project root for license information.
 */

#ifndef CHIME_CORE_PLATFORM_STATUS_H_
#define CHIME_CORE_PLATFORM_STATUS_H_

#include <sys/types.h>

#include "chime/core/platform/error_codes.pb.h"

namespace chime {

namespace errors {
typedef chime::error::Code Code;
}

struct SourceLocation {
  u_int32_t line;
  std::string file_name;

  static SourceLocation current(u_int32_t line = 0,
                                const char *file_name = nullptr) {
    SourceLocation location;
    location.line = line;
    location.file_name = file_name;
    return location;
  }
};

class Status final {
 public:
  Status() {}
};

}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_STATUS_H_
