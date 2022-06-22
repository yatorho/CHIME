// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
#define CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_

#include "chime/core/platform/env.hpp"

namespace chime {
namespace platform {

class ThreadPoolImpl;


/// \brief Real implementation for thread pool.
class ThreadPoolImpl {
 public:
  ThreadPoolImpl(Env *env, const ThreadOptions &thread_options,
                 const std::string &name, int64_t num_threads,
                 bool low_latency_hint);

};

}  // namespace platform
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_THREADPOOL_IMPL_H_
