// Copyright by 2022.6 chime
// author: yatorho

#include "chime/core/platform/threadpool.h"

#include <functional>
#include <memory>

#include "chime/core/framework/common.hpp"

namespace chime {
namespace platform {
// struct TaskImpl {
//   std::function<void()> f;
//   uint64 trace_id;
// };
// class Task {
//   std::unique_ptr<TaskImpl> f;
// };

int64_t ThreadPool::num_shards_used_by_fixed_block_size_scheduling(
    int64_t total, const int64_t block_size) const {
  if (block_size <= 0 || total <= 1 || total <= block_size ||
      num_threads() == 1)
    return 1;
  return (total + block_size - 1) / block_size;
}

}  // namespace platform
}  // namespace chime
