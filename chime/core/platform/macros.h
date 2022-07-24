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

#ifdef __has_builtin
#define CHIME_HAS_BUILTIN(x) __has_builtin(x)
#else
#define CHIME_HAS_BUILTIN(x) 0
#endif

#if defined(__has_cpp_attribute)
#define CHIME_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#else
#define CHIME_HAS_CPP_ATTRIBUTE(x) 0
#endif 

#if CHIME_HAS_BUILTIN(__builtin_expect) || (defined(__GNUC__) && __GNUC__ >= 3)
#define CHIME_PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define CHIME_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else 
#define CHIME_PREDICT_FALSE(x) (x)
#define CHIME_PREDICT_TRUE(x) (x)
#endif

#endif  // CHIME_CORE_PLATFORM_MACROS_H_
