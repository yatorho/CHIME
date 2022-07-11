// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#include "chime/core/runtime/process_state.h"

#include "chime/core/framework/device_types.h"
#include "chime/core/platform/strcat.h"

namespace chime {
namespace memory {

ProcessState *ProcessState::singleton() {
  static ProcessState *instance = new ProcessState();
  return instance;
}

ProcessState::ProcessState() : _numa_enabled(false) {}

std::string ProcessState::MemDesc::debug_string() const {
  return cestring::CEStrCat(
      (loc == DeviceType::CENTRAL_PROCESSING_UNIT ? "CPU" : "GPU"), dev_index,
      ", dma: ", gpu_registered, ", nic: ", nic_registered);
}



}  // namespace memory
}  // namespace chime
