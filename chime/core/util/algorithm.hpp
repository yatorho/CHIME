// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_UTIL_ALGORITHM_HPP_
#define CHIME_CORE_UTIL_ALGORITHM_HPP_

namespace chime {
namespace algorithm {

template <typename Tp, typename InputIterator = Tp *>
Tp accumulate(InputIterator first, InputIterator last, Tp init) {
  for (; first != last; ++first) {
    init += *first;
  }
  return init;
}

}  // namespace algorithm
}  // namespace chime

#endif  // CHIME_CORE_UTIL_ALGORITHM_HPP_
