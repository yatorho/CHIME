// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_MUTEX_H_
#define CHIME_CORE_PLATFORM_MUTEX_H_

#include <condition_variable>
#include <mutex>

namespace chime {
namespace concurrent {

using Mutex = std::mutex;
using ConditionVariable = std::condition_variable;
using UniqueLock = std::unique_lock<Mutex>;
using LockGurd = std::lock_guard<Mutex>;

}  // namespace concurrent

using mutex = concurrent::Mutex;
using condition_variable = concurrent::ConditionVariable;
using mutex_lock = concurrent::UniqueLock;
using lock_guard = concurrent::LockGurd;

}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_MUTEX_H_
