// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/runtime/process_state.h"

#include "chime/core/framework/device_types.h"
#include "chime/core/memory/allocator.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/numa.h"
#include "chime/core/platform/strcat.h"

namespace chime {
namespace memory {

ProcessState *ProcessState::singleton() {
  static ProcessState *instance = new ProcessState();
  return instance;
}

ProcessState::ProcessState() : _numa_enabled(false) {}

std::string ProcessState::MemDesc::DebugString() const {
  return cestring::CEStrCat(
      (loc == DeviceType::CENTRAL_PROCESSING_UNIT ? "CPU" : "GPU"), dev_index,
      ", dma: ", gpu_registered, ", nic: ", nic_registered);
}

ProcessState::MemDesc ProcessState::PtrType(const void *ptr) {
  auto iter = _mem_desc.find(ptr);
  if (iter != _mem_desc.end()) {
    return iter->second;
  }
  return MemDesc();
}

}  // namespace memory
}  // namespace chime
