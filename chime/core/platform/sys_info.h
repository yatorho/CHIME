// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_SYS_INFO_H_
#define CHIME_CORE_PLATFORM_SYS_INFO_H_

#include <string>

namespace chime {
namespace port {

std::string hostname();

std::string job_name();  // From `CE_JOB_NAME` environment variable.

}
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_SYS_INFO_H_
