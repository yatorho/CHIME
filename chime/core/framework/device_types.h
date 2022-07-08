// Copyright by 2022.7 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_DEVICE_TYPES_H_
#define CHIME_CORE_FRAMEWORK_DEVICE_TYPES_H_

namespace chime {

enum class DeviceType {
  CENTRAL_PROCESSING_UNIT,   // refers to CPU
  GRAPHICS_PROCESSING_UNIT,  // refers to GPU
  TENSOR_PROCESSING_UNIT,    // refers to TPU
  UNKNOWN,                   // refers to unknown device type
};
}

#endif  // CHIME_CORE_FRAMEWORK_DEVICE_TYPES_H_
