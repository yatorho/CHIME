// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_CONTAINER_WRAPPER_HPP_
#define CHIME_CORE_UTIL_CONTAINER_WRAPPER_HPP_

#include <sys/types.h>

namespace chime {
namespace util {
template <typename Tp, typename container>
struct ContainerWrapper {
  typedef Tp value_type;
  typedef ::size_t size_type;
  typedef Tp &reference;
  typedef const Tp &const_reference;
  typedef Tp *pointer;
  typedef const Tp *const_pointer;
  typedef container container_type;
};

}  // namespace util
}  // namespace chime

#endif  // CHIME_CORE_UTIL_CONTAINER_WRAPPER_HPP_
