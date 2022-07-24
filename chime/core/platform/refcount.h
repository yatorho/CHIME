// Copyright by 2022.7 chime. All rights reserved.
// author: yatorho

#ifndef CHIME_CORE_PLATFORM_REFCOUNT_H_
#define CHIME_CORE_PLATFORM_REFCOUNT_H_

#include <atomic>
#include <functional>
#include <memory>

#include "chime/core/framework/types.hpp"
#include "chime/core/platform/logging.hpp"
#include "chime/core/platform/macros.h"
#include "chime/core/platform/mutex.h"
#include "chime/core/platform/thread_annotations.h"

namespace chime {
namespace core {

class RefCounted {
 public:
  RefCounted();

  void Ref() const;

  /// Decrements reference count by one. If the count remains positive, returns
  /// false.  When the count reaches zero, returns true and deletes this, in
  /// which case the caller must not access the object afterward.
  bool Unref() const;

  /// Gets the current reference count.
  int64_t RefCount() const;

  /// Return whether the reference count is one.
  /// If the reference count is used in the conventional way, a
  /// reference count of 1 implies that the current thread owns the
  /// reference and no other thread shares it.
  /// This call performs the test for a reference count of one, and
  /// performs the memory barrier needed for the owning thread
  /// to act on the object, knowing that it has exclusive access to the
  /// object.
  bool RefCountIsOne() const;

 protected:
  virtual ~RefCounted();

  /// Increments reference count by one if the object is not being destructed.
  /// This function is used by WeakRefCounted for securely acquiring a
  /// strong reference. It is only safe to call this as part of the weak
  /// reference implementation.
  bool TryRef() const;

 private:
  mutable std::atomic_int64_t _ref;

  CHIME_DISALLOW_COPY_AND_ASSIGN(RefCounted);
};

/// A deleter class to form a std::unique_ptr that unrefs objects.
struct RefCountDeleter {
  void operator()(const RefCounted *o) const {
    if (CHIME_PREDICT_TRUE(o)) o->Unref();
  }
};

template <typename T>
using RefCountPtr = std::unique_ptr<T, RefCountDeleter>;

class ScopedUnref {
 public:
  explicit ScopedUnref(const RefCounted *o) : _obj(o) {}
  ~ScopedUnref() {
    if (CHIME_PREDICT_TRUE(_obj)) _obj->Unref();
  }

 private:
  const RefCounted *_obj;

  CHIME_DISALLOW_COPY_AND_ASSIGN(ScopedUnref);
};

template <typename T>
class WeakPtr;

using WeakNotifyFn = std::function<void()>;

inline RefCounted::RefCounted() : _ref(1) {}

inline RefCounted::~RefCounted() { DCHECK_EQ(_ref.load(), 0); }

inline void RefCounted::Ref() const {
  // Ref() uses relaxed order because it is never called with old_ref == 0.
  // When old_ref >= 1, no actions depend on the new value of ref.
  int64_t old_ref = _ref.fetch_add(1, std::memory_order_relaxed);
  DCHECK_GT(old_ref, 0);
}

inline bool RefCounted::TryRef() const {
  int64_t old_ref = _ref.load();
  while (old_ref != 0) {
    if (_ref.compare_exchange_weak(old_ref, old_ref + 1)) {
      return true;
    }
  }
  // Already destructing, cannot increase ref.
  return false;
}

class WeakRefCounted : public RefCounted {
 public:
  int64_t WeakRefCount() const { return _data->RefCount() - 1; }

 protected:
  ~WeakRefCounted() override { _data->Notify(); }

 private:
  struct WeakRefData : public RefCounted {
    explicit WeakRefData(WeakRefCounted *p) : ptr(p), nex_notifier_id(1) {}

    mutable mutex mu;
    WeakRefCounted *ptr CHIME_GUARDED_BY(mu);
    std::map<int64_t, WeakNotifyFn> notifiers;
    int64_t nex_notifier_id;

    void Notify() {
      mutex_lock l(mu);
      while (!notifiers.empty()) {
        auto iter = notifiers.begin();

        WeakNotifyFn notify_fn = std::move(iter->second);
        notifiers.erase(iter);

        mu.unlock();
        notify_fn();
        mu.lock();
      }
      ptr = nullptr;
    }

    WeakRefCounted *GetNewRef() {
      mutex_lock l(mu);
      if (ptr != nullptr && ptr->TryRef()) {
        return ptr;
      }
      return nullptr;
    }

    int64_t AddNotifier(WeakNotifyFn nofify_fn) {
      mutex_lock l(mu);
      if (ptr == nullptr) {
        return 0;
      }
      int64_t notifier_id = nex_notifier_id++;
      notifiers.emplace(notifier_id, std::move(nofify_fn));
      return notifier_id;
    }

    void RemoveNotifier(int64_t notifier_id) {
      mutex_lock l(mu);
      notifiers.erase(notifier_id);
    }
  };

  RefCountPtr<WeakRefData> _data{new WeakRefData(this)};

  template <typename T>
  friend class WeakPtr;

  friend struct WeakRefData;
};

template <typename T>
class WeakPtr {
 public:
  explicit WeakPtr(WeakRefCounted *ptr, WeakNotifyFn notify_fn = nullptr)
      : _data(nullptr), _notifier_id(0) {
    if (CHIME_PREDICT_TRUE(ptr != nullptr)) {
      ptr->_data->Ref();
      _data.reset(ptr->_data.get());
      if (notify_fn) {
        _notifier_id = _data->AddNotifier(std::move(notify_fn));
      }
    }
  }

  ~WeakPtr() {
    if (_data != nullptr && _notifier_id != 0) {
      _data->RemoveNotifier(_notifier_id);
    }
  }

  WeakPtr(WeakPtr &&other) {
    _data = std::move(other._data);
    _notifier_id = other._notifier_id;
    other._notifier_id = 0;
  }

  WeakPtr &operator=(WeakPtr &&other) {
    if (CHIME_PREDICT_TRUE(this != &other)) {
      if (_data != nullptr && _notifier_id != 0) {
        _data->RemoveNotifier(_notifier_id);
      }
      _data = std::move(other._data);
      _notifier_id = other._notifier_id;
      other._notifier_id = 0;
    }
    return *this;
  }

  RefCountPtr<T> GetNewRef() const {
    RefCountPtr<T> ref;
    if (_data != nullptr) {
      WeakRefCounted *ptr = _data->GetNewRef();
      ref.reset(static_cast<T *>(ptr));
    }
    return std::move(ref);
  }

 private:
  RefCountPtr<WeakRefCounted::WeakRefData> _data;
  int64_t _notifier_id;

  CHIME_DISALLOW_COPY_AND_ASSIGN(WeakPtr);
};

inline bool RefCounted::Unref() const {
  DCHECK_GT(_ref.load(), 0);

  if (_ref.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    delete this;
    return true;
  }
  return false;
}

inline int64_t RefCounted::RefCount() const {
  return _ref.load(std::memory_order_acquire);
}

inline bool RefCounted::RefCountIsOne() const {
  return (_ref.load(std::memory_order_acquire) == 1);
}

}  // namespace core
}  // namespace chime

#endif  // CHIME_CORE_PLATFORM_REFCOUNT_H_
