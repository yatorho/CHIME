// Copyright by 2022.6 chime
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_MACROS_H_
#define CHIME_CORE_PLATFORM_MACROS_H_

#define CHIME_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName &) = delete;           \
  TypeName &operator=(const TypeName &) = delete

#define CHIME_DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName() = delete;                                 \
  TypeName(const TypeName &) = delete;                 \
  TypeName(TypeName &&) = delete;                      \
  TypeName &operator=(const TypeName &) = delete;      \
  TypeName &operator=(TypeName &&) = delete

#define CHIME_MUST_USE_RESULT __attribute__((warn_unused_result))

#endif  // CHIME_CORE_PLATFORM_MACROS_H_
