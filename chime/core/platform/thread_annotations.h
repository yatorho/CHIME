// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_THREAD_ANNOTATIONS_H_
#define CHIME_CORE_PLATFORM_THREAD_ANNOTATIONS_H_

#if defined(__GNUC__) && (!defined(SWIG))
#define CHIME_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(x) __attribute__((x))
#else
#define CHIME_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(x)  // no-op
#endif

/// Document if a shared variable/field needs to be protected by a mutex.
/// CHIME_GUARDED_BY allows the user to specify a particular mutex that should
/// be held when accessing the annotated variable.  GUARDED_VAR indicates that
/// a shared variable is guarded by some unspecified mutex, for use in rare
/// cases where a valid mutex expression cannot be specified.

#define CHIME_GUARDED_BY(x) \
  CHIME_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(guarded_by(x))
#define GUARDED_VAR  // no-op


#define CHIME_EXCLUSIVE_LOCKS_REQUIRED(x) \
  CHIME_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(exclusive_locks_required(x))

#endif  // CHIME_CORE_PLATFORM_THREAD_ANNOTATIONS_H_
