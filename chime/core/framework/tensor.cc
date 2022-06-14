// Copyright by 2022.5 chime
// author: yatorho

#include "chime/core/framework/tensor.h"

#include <algorithm>
#include <memory>
#include <string>
#include <type_traits>

#include "chime/core/framework/common.hpp"
#include "chime/core/framework/syncedmem.hpp"
#include "chime/core/framework/tensor_shape.hpp"
#include "chime/core/framework/type.hpp"
#include "chime/core/memory/mem_optimizer.h"
#include "chime/core/schema/tensor.pb.h"
#include "chime/core/schema/types.pb.h"

namespace chime {
void log_unexpected_size(int64_t actual, int64_t expected) {
  LOG(ERROR) << "Input size was " << actual << " and expected " << expected;
}

template <typename T>
struct Helper {
  static_assert(IsValidDataType<T>::value, "T is not a simple type.");
  typedef google::protobuf::RepeatedField<T> repeated_field_type;

  template <typename Source>
  static Tensor::DataPtr decode(Tensor::MemOp &mem_op, const Source &in,
                                utens_t n) {
    if (in.size() != sizeof(T) * n) {
      log_unexpected_size(in.size(), sizeof(T) * n);
      return nullptr;
    }

    Tensor::DataPtr buf;
    buf.reset(new SyncedMemory(mem_op, sizeof(T) * n));
    // DCHECK(in.data());
    const void *mem_ptr = (const void *)in.data();
    if (mem_ptr == nullptr) return nullptr;
    mem_op.memcpy(buf->mutable_host_mem(), mem_ptr, sizeof(T) * n,
                  Tensor::MemOp::COPY_FROM_HOST_MEMORY);
    return buf;
  }

  template <typename Destination>
  static void encode(Tensor::DataPtr in, utens_t n, Destination *out) {
    DCHECK_EQ(in->size(), sizeof(T) * n);
    void *ptr = nullptr;
    memory::default_allocator.malloc(
        &ptr, in->size(), memory::MemoryOptimizer::MALLOC_FROM_HOST_MEMORY);
    in->dump_to(ptr);

    out->assign(static_cast<const char *>(ptr), in->size());
    // DCHECK_NE(out->data(), ptr);
    memory::default_allocator.free(
        ptr, memory::MemoryOptimizer::FREE_FROM_HOST_MEMORY);
    DCHECK_EQ(out->size(), in->size());
  }
};

template <typename T>
struct ProtoHelper {};

// For a C++ type `T` (float, double, int32, etc.), the repeated field
// `N`_val (float32_val, float64_val, etc.) of type `F` (float, int32, uint64,
// etc.) in the TensorProto is used for serializing the tensor of type `T`.
#define PROTO_TRAITS(T, F, N)                                           \
  template <>                                                           \
  struct ProtoHelper<T> {                                               \
    typedef Helper<F>::repeated_field_type field_type;                  \
    static field_type::const_iterator begin(const TensorProto &proto) { \
      return proto.N##_val().begin();                                   \
    }                                                                   \
    static size_t num_elements(const TensorProto &proto) {              \
      return proto.N##_val().size();                                    \
    }                                                                   \
    static void fill(const T *data, size_t n, TensorProto *proto) {     \
      typename ProtoHelper<T>::field_type copy(data, data + n);         \
      proto->mutable_##N##_val()->Swap(&copy);                          \
    }                                                                   \
  }
PROTO_TRAITS(float, float, float32);
PROTO_TRAITS(double, double, float64);
PROTO_TRAITS(int32, int32, int);
PROTO_TRAITS(int16, int32, int);
PROTO_TRAITS(int8, int32, int);
PROTO_TRAITS(uint32, uint32, uint);
PROTO_TRAITS(uint16, uint32, uint);
PROTO_TRAITS(uint8, uint32, uint);
PROTO_TRAITS(bool, bool, bool);
#undef PROTO_TRAITS

template <>
struct ProtoHelper<int64> {
  static google::protobuf::RepeatedField<
      google::protobuf::int64>::const_iterator
  begin(const TensorProto &proto) {
    return proto.int64_val().begin();
  }
  static size_t num_elements(const TensorProto &proto) {
    return proto.int64_val().size();
  }
  static void fill(const int64 *data, size_t n, TensorProto *proto) {
    google::protobuf::RepeatedField<google::protobuf::int64> copy(data,
                                                                  data + n);
    proto->mutable_int64_val()->Swap(&copy);
  }
};

template <>
struct ProtoHelper<uint64> {
  static google::protobuf::RepeatedField<
      google::protobuf::uint64>::const_iterator
  begin(const TensorProto &proto) {
    return proto.uint64_val().begin();
  }
  static size_t num_elements(const TensorProto &proto) {
    return proto.uint64_val().size();
  }
  static void fill(const uint64 *data, size_t n, TensorProto *proto) {
    google::protobuf::RepeatedField<google::protobuf::uint64> copy(data,
                                                                   data + n);
    proto->mutable_uint64_val()->Swap(&copy);
  }
};

template <>
struct ProtoHelper<complex64> {
  typedef Helper<float>::repeated_field_type field_type;
  static const complex64 *begin(const TensorProto &proto) {
    return reinterpret_cast<const complex64 *>(proto.scomplex_val().data());
  }
  static size_t num_elements(const TensorProto &proto) {
    return proto.scomplex_val().size() / 2;
  }
  static void fill(const complex64 *data, size_t n, TensorProto *proto) {
    const float *p = reinterpret_cast<const float *>(data);
    field_type copy(p, p + n * 2);
    proto->mutable_scomplex_val()->Swap(&copy);
  }
};

template <>
struct ProtoHelper<complex128> {
  typedef Helper<double>::repeated_field_type field_typ;
  static const complex128 *begin(const TensorProto &proto) {
    return reinterpret_cast<const complex128 *>(proto.dcomplex_val().data());
  }
  static size_t num_elements(const TensorProto &proto) {
    return proto.dcomplex_val().size();
  }
  static void fill(const complex128 *data, size_t n, TensorProto *proto) {
    const double *p = reinterpret_cast<const double *>(data);
    field_typ copy(p, p + n * 2);
    proto->mutable_dcomplex_val()->Swap(&copy);
  }
};

template <>
struct ProtoHelper<string> {
  static google::protobuf::RepeatedPtrField<
      std::basic_string<char>>::const_iterator
  begin(const TensorProto &proto) {
    return proto.string_val().begin();
  }
  static size_t num_elements(const TensorProto &proto) {
    return proto.string_val().size();
  }
  static void fill(const std::basic_string<char> *data, size_t n,
                   TensorProto *proto) {
    string b = "123";
    std::basic_string<char> *s = &b;
    google::protobuf::RepeatedPtrField<std::basic_string<char>> copy(
        data->data(), (data + n)->data());
    proto->mutable_string_val()->Swap(&copy);
  }
};

template <typename T>
Tensor::DataPtr from_proto_field(Tensor::MemOp &mem_op, const TensorProto &in,
                                 utens_t n) {
  CHECK_GT(n, 0ull);
  Tensor::DataPtr buf(new SyncedMemory(mem_op, n));

  T *data = static_cast<T *>(buf->mutable_host_mem());
  if (data == nullptr) return nullptr;

  const utens_t in_n = ProtoHelper<T>::num_elements(in);
  if (in_n <= 0) {
    std::fill_n(data, n, T());
  } else {
    auto begin = ProtoHelper<T>::begin(in);
    if (n < in_n) {
      std::copy_n(begin, n, data);
    } else {
      std::copy_n(begin, in_n, data);
      if (std::is_trivially_copyable<T>::value) {
        const T last = *(data + in_n - 1);
        std::fill_n(data + in_n, n - in_n, last);
      } else {
        const T &last = *(data + in_n - 1);
        std::fill_n(data + in_n, n - in_n, last);
      }
    }
  }
  return buf;
}

template <typename T>
void to_proto_field(const Tensor::DataPtr &in, utens_t n, TensorProto *out) {
  const T *data = static_cast<const T *>(in->host_mem());
  ProtoHelper<T>::fill(data, n, out);
}

Tensor::Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape,
               DeviceName d_name)
    : _dtype(dtype), _shape(shape), _dname(d_name) {
  _buffer.reset(
      new SyncedMemory(mem_op, _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(MemOp &mem_op, DataType dtype, const TensorShape &shape)
    : _dtype(dtype), _shape(shape), _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(
      new SyncedMemory(mem_op, _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype, const TensorShape &shape)
    : _dtype(dtype), _shape(shape), _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator,
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor(DataType dtype)
    : _dtype(dtype),
      _shape(std::move(TensorShape())),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator,
                                 _shape.num_elements() * dtype_size(dtype)));
}

Tensor::Tensor()
    : _dtype(DT_INVALID),
      _shape(std::move(TensorShape())),
      _dname(GRAPHICS_PROCESSING_UNIT) {
  _buffer.reset(new SyncedMemory(memory::default_allocator, 0ul));
}

Tensor::Tensor(const Tensor &other)
    : _dtype(other.dtype()),
      _shape(other.shape()),
      _dname(other.device_name()) {
  _buffer = other._buffer;
}

Tensor::Tensor(Tensor &&other)
    : _dtype(other.dtype()),
      _shape(std::move(other.shape())),
      _dname(other.device_name()) {
  _buffer = other._buffer;
}

Tensor &Tensor::operator=(const Tensor &other) {
  _dtype = other.dtype();
  _shape = other.shape();
  _dname = other.device_name();
  _buffer = other._buffer;
  return *this;
}

Tensor &Tensor::operator=(Tensor &&other) {
  _dtype = other.dtype();
  _shape = TensorShape(std::move(other.shape()));
  _dname = other.device_name();
  _buffer = other._buffer;
  return *this;
}

Tensor::~Tensor() {}

bool Tensor::operator==(const Tensor &other) { NOT_IMPLEMENTED; }

bool Tensor::operator==(Tensor &&other) { NOT_IMPLEMENTED; }

utens_t Tensor::operator[](utens_t d) const { NOT_IMPLEMENTED; }

bool Tensor::is_initialized() const {
  return head() != SyncedMemory::UNINITIALIZED;
}

mems_t Tensor::total_bytes() const {
  return _shape.num_elements() * dtype_size(_dtype);
}

mems_t Tensor::allocated_bytes() const {
  switch (head()) {
    case SyncedMemory::UNINITIALIZED:
      return 0ull;
    case SyncedMemory::HEAD_AT_HOST:
      return _buffer->own_host_mem() ? _buffer->size() : 0ull;
    case SyncedMemory::HEAD_AT_DEVICE:
      return _buffer->own_device_mem() ? _buffer->size() : 0ull;
    case SyncedMemory::SYNCED:
      return 2ull * _buffer->size();
    default:
      LOG(FATAL) << "Unknown SyncedHead status!";
  }
}

void *Tensor::buffer(OperateFrom of) {
  return of == HOST ? _buffer->mutable_host_mem()
                    : _buffer->mutable_device_mem(device_name());
}

bool Tensor::is_legal_shape() const { return _shape.check_legality(); }

bool Tensor::is_scalar() const { return _shape.dims() == 0 && check_dtype(); }

bool Tensor::check_dtype() const { return _dtype != DT_INVALID; }

utens_t Tensor::ref_count() const { return _buffer.use_count(); }

#define SINGLE_ARG(...) __VA_ARGS__
#define CASE(TYPE, STMTS)             \
  case DataTypeToEnum<TYPE>::value: { \
    typedef TYPE T;                   \
    STMTS;                            \
    break;                            \
  }
#define CASES_WITH_DEFAULT(TYPE_ENUM, STMTS, INVALID, DEFAULT) \
  switch (TYPE_ENUM) {                                         \
    CASE(float32, SINGLE_ARG(STMTS))                           \
    CASE(float64, SINGLE_ARG(STMTS))                           \
    CASE(int32, SINGLE_ARG(STMTS))                             \
    CASE(uint8, SINGLE_ARG(STMTS))                             \
    CASE(uint16, SINGLE_ARG(STMTS))                            \
    CASE(uint32, SINGLE_ARG(STMTS))                            \
    CASE(uint64, SINGLE_ARG(STMTS))                            \
    CASE(int16, SINGLE_ARG(STMTS))                             \
    CASE(int8, SINGLE_ARG(STMTS))                              \
    CASE(string, SINGLE_ARG(STMTS))                            \
    CASE(complex64, SINGLE_ARG(STMTS))                         \
    CASE(complex128, SINGLE_ARG(STMTS))                        \
    CASE(int64, SINGLE_ARG(STMTS))                             \
    CASE(bool, SINGLE_ARG(STMTS))                              \
    case DT_INVALID:                                           \
      INVALID;                                                 \
      break;                                                   \
    default:                                                   \
      DEFAULT;                                                 \
      break;                                                   \
  }

#define CASES(TYPE_ENUM, STMTS)                                      \
  CASES_WITH_DEFAULT(TYPE_ENUM, STMTS, LOG(FATAL) << "Type not set"; \
                     , LOG(FATAL) << "Unexpected type: " << TYPE_ENUM;)

bool Tensor::from_proto(const TensorProto &proto) {
  return from_proto(memory::default_allocator, proto);
}

bool Tensor::from_proto(MemOp &mem_op, const TensorProto &proto) {
  if (!TensorShape::is_valid(proto.tensor_shape())) return false;
  if (proto.dtype() == DT_INVALID) return false;

  TensorShape shape;
  shape.from_proto(proto.tensor_shape());
  const utens_t N = shape.num_elements();

  DataPtr buffer;

  if (N > 0 && proto.dtype()) {
    bool dtype_error = false;
    if (!proto.tensor_content().empty()) {
      CASES_WITH_DEFAULT(
          proto.dtype(),
          buffer = Helper<T>::decode(mem_op, proto.tensor_content(), N),
          dtype_error = true, dtype_error = true);
    } else {
      CASES_WITH_DEFAULT(proto.dtype(),
                         buffer = from_proto_field<T>(mem_op, proto, N),
                         dtype_error = true, dtype_error = true);
    }
    if (dtype_error || buffer == nullptr) return false;
  } else {
    bool dtype_error = false;
    CASES_WITH_DEFAULT(proto.dtype(), break, dtype_error = true,
                       dtype_error = true);
    if (dtype_error) return false;
    return false;
  }

  _shape = shape;
  _set_dtype(proto.dtype());
  _buffer = buffer;
  return true;
}

void Tensor::as_proto_tensor_content(TensorProto *proto) const {
  proto->Clear();
  proto->set_dtype(dtype());
  _shape.as_proto(proto->mutable_tensor_shape());
  if (_buffer) {
    CASES(dtype(), Helper<T>::encode(_buffer, _shape.num_elements(),
                                     proto->mutable_tensor_content()));
  }
}

void Tensor::as_proto_field(TensorProto *proto) const {
  proto->Clear();
  proto->set_dtype(dtype());
  _shape.as_proto(proto->mutable_tensor_shape());
  if (_buffer) {
    CASES(dtype(), to_proto_field<T>(_buffer, _shape.num_elements(), proto));
  }
}

}  // namespace chime
