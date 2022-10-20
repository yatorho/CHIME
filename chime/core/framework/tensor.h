// Copyright 2022.5 chime
// author: yatorho

#ifndef CHIME_CORE_FRAMEWORK_TENSOR_H_
#define CHIME_CORE_FRAMEWORK_TENSOR_H_

#include "chime/core/platform/refcount.h"

namespace chime {

class TensorBuffer;

class TensorBuffer : public core::RefCounted {
 public:
  TensorBuffer(void *ptr) : _data(ptr) {}

  virtual ~TensorBuffer() {}

  void *Data() const { return _data; }

  template <typename T>
  T *Base() const {
    return reinterpret_cast<T *>(_data);
  }

  virtual size_t Size() const = 0;

  virtual TensorBuffer * RootBuffer() = 0;




 private:
  void *const _data;
};

}  // namespace chime

#endif  // CHIME_CORE_FRAMEWORK_TENSOR_H_
