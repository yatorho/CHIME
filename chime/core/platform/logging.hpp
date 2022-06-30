// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_LOGGING_HPP_
#define CHIME_CORE_PLATFORM_LOGGING_HPP_

#include <glog/logging.h>

#define CHIME_NOT_IMPLEMENTED LOG(FATAL) << "Not implemented yet."

#endif  // CHIME_CORE_PLATFORM_LOGGING_HPP_
