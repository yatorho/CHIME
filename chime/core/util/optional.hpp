// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_OPTIONAL_HPP_
#define CHIME_CORE_UTIL_OPTIONAL_HPP_

#include <type_traits>
#include <utility>

#include "chime/core/platform/logging.hpp"

namespace chime {
namespace util {

/// A tag type to represent an empty optional
struct nullopt_t {
  struct do_not_use {};
  constexpr explicit nullopt_t(do_not_use, do_not_use) noexcept {}
};

/// Represents an empty optional
static constexpr nullopt_t nullopt{nullopt_t::do_not_use{},
                                   nullopt_t::do_not_use{}};

template <typename Tp>
class Optional {
  using data_type =
      typename std::aligned_storage<sizeof(Tp),
                                    std::alignment_of<Tp>::value>::type;
  using type_ptr = Tp *;

 public:
  Optional() : _inited(false) {}

  Optional(const Tp &value) { create(value); }

  Optional(Tp &&value) { create(std::move(value)); }

  Optional(const nullopt_t &null) : Optional() {}

  Optional(nullopt_t &&null) : Optional() {}

  ~Optional() { destroy(); }

  Optional(const Optional &other) : _inited(false) {
    if (other._inited) assign(other);
  }

  Optional(Optional &&other) : _inited(false) {
    if (other._inited) {
      assign(std::move(other));
    }
  }

  Optional &operator=(const Optional &other) {
    assign(other);
    return *this;
  }

  Optional &operator=(const nullopt_t &null) {
    destroy();
    return *this;
  }

  Optional &operator=(Optional &&other) {
    assign(std::move(other));
    return *this;
  }

  Optional &operator=(nullopt_t &&null) {
    destroy();
    return *this;
  }

  template <class... Args>
  void emplace(Args &&...args) {
    destroy();
    create(std::forward<Args>(args)...);
  }

  bool is_init() const { return _inited; }

  explicit operator bool() const { return is_init(); }

  Tp &operator*() { return *reinterpret_cast<type_ptr>(&_data); }

  const Tp &operator*() const {
    return *reinterpret_cast<const type_ptr>(&_data);
  }

  Tp *operator->() {
    return is_init() ? reinterpret_cast<type_ptr>(&_data) : nullptr;
  }

  const Tp *operator->() const {
    return is_init() ? reinterpret_cast<const type_ptr>(&_data) : nullptr;
  }

  Tp &value() {
    if (is_init()) return **this;
    LOG(FATAL) << "Optional container have not been initialized.";
  }

  const Tp &value() const {
    if (is_init()) return **this;
    LOG(FATAL) << "Optional container have not been initialized.";
  }

  bool operator==(const Optional<Tp> &rhs) const {
    return (!bool(*this)) != (!rhs)
               ? false
               : (!bool(*this) ? true : (*(*this)) == (*rhs));
  }

  bool operator<(const Optional<Tp> &rhs) const {
    return !rhs ? false : (!bool(*this) ? true : (*(*this) < (*rhs)));
  }

  bool operator!=(const Optional<Tp> &rhs) { return !(*this == (rhs)); }

 private:
  template <class... Args>
  void create(Args &&...args) {
    new (&_data) Tp(std::forward<Args>(args)...);
    _inited = true;
  }

  void destroy() {
    if (_inited) {
      _inited = false;
      ((type_ptr)(&_data))->~Tp();
    }
  }

  void assign(const Optional &other) {
    if (other._inited) {
      _copy(other._data);
      _inited = true;
    } else
      destroy();
  }

  void assign(Optional &&other) {
    if (other._inited) {
      _move(std::move(other._data));
      _inited = true;
      other.destroy();
    } else
      destroy();
  }

  void _move(data_type &&val) {
    destroy();
    new (&_data) Tp(std::move(*(type_ptr)(&val)));
  }

  void _copy(const data_type &val) {
    destroy();
    new (&_data) Tp(*(type_ptr)(&val));
  }

 private:
  bool _inited;
  data_type _data;
};

}  // namespace util
}  // namespace chime

#endif  // CHIME_CORE_UTIL_OPTIONAL_HPP_
