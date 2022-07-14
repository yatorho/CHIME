// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_STRCAT_H_
#define CHIME_CORE_PLATFORM_STRCAT_H_

#include <initializer_list>

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/macros.h"

namespace chime {
namespace cestring {

class StringPiece {
  /// TODO(yatorho): implement
};

class AlphaNum {
 public:
  AlphaNum(int8 i8);
  AlphaNum(uint8 u8);
  AlphaNum(int16 i16);
  AlphaNum(uint16 u16);
  AlphaNum(int32 i32);
  AlphaNum(int64 i64);
  AlphaNum(uint32 u32);
  AlphaNum(uint64 u64);
  AlphaNum(float32 f32);
  AlphaNum(float64 f64);
  AlphaNum(float128 f128);
  AlphaNum(bool b);
  AlphaNum(const char *s);
  AlphaNum(const std::string &s);
  AlphaNum(complex64 c64);
  AlphaNum(complex128 c128);

  StringPiece piece() const;

 private:
  StringPiece _piece;
  CHIME_DISALLOW_COPY_AND_ASSIGN(AlphaNum);
};

/// Might be optimized for <= 4 args.
std::string CEStrCat(const AlphaNum &a) CHIME_MUST_USE_RESULT;
std::string CEStrCat(const AlphaNum &a,
                     const AlphaNum &b) CHIME_MUST_USE_RESULT;
std::string CEStrCat(const AlphaNum &a, const AlphaNum &b,
                     const AlphaNum &c) CHIME_MUST_USE_RESULT;
std::string CEStrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
                     const AlphaNum &d) CHIME_MUST_USE_RESULT;

void CEStrAppend(std::string *s, const AlphaNum &a);
void CEStrAppend(std::string *s, const AlphaNum &a, const AlphaNum &b);
void CEStrAppend(std::string *s, const AlphaNum &a, const AlphaNum &b,
                 const AlphaNum &c);
void CEStrAppend(std::string *s, const AlphaNum &a, const AlphaNum &b,
                 const AlphaNum &c, const AlphaNum &d);

namespace internal {  /// Do not call directly - this is not part of public API.

std::string cat_piece(std::initializer_list<StringPiece> pieces);
void append_pieces(std::string *out, std::initializer_list<StringPiece> pieces);

}  // namespace internal

/// Support for 5 or more arguments.
template <typename... Args>
std::string CEStrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
                     const AlphaNum &d, const AlphaNum &e,
                     Args... args) CHIME_MUST_USE_RESULT;

template <typename... Args>
std::string CEStrCat(const AlphaNum &a, const AlphaNum &b, const AlphaNum &c,
                     const AlphaNum &d, const AlphaNum &e, Args... args) {
  return internal::cat_piece({a.piece(), b.piece(), c.piece(), d.piece(),
                              e.piece(),
                              static_cast<const AlphaNum &>(args).piece()...});
}

template <typename... Args>
inline void CEStrAppend(std::string *s, const AlphaNum &a, const AlphaNum &b,
                 const AlphaNum &c, const AlphaNum &d, const AlphaNum &e,
                 Args... args) {
  internal::append_pieces(
      s, {a.piece(), b.piece(), c.piece(), d.piece(), e.piece(),
          static_cast<const AlphaNum &>(args).piece()...});
}

}  // namespace cestring
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_STRCAT_H_
