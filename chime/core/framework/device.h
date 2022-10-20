// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_DEVICE_H_
#define CHIME_CORE_FRAMEWORK_DEVICE_H_

#include <cstdint>

#include "chime/core/framework/device_types.h"
#include "chime/core/framework/device_types.pb.h"

namespace chime {

class Device final {
 public:
  Device() {}

  Device(int64_t id, DeviceType type) : _device_id(id), _device_type(type) {}

 private:
  int64_t _device_id;
  DeviceType _device_type;
  std::string _device_name;
};

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_DEVICE_H_
