#pragma once
#include <utility>

#include "palkia/common.h"
#include "palkia/remote_ptr.h"

namespace palkia {

void Init(int* argc, char** argv[]);

template <typename T>
class RemoteRef {
 public:
  explicit RemoteRef<T>(RemotePtr<T>& ref) : ref_(ref) {
    ref_.inc_refcnt();
  }

  // copy constructor
  RemoteRef<T>(RemoteRef<T>& other) {
    *this = other;
  }

  // copy assignment
  RemoteRef<T>& operator=(RemoteRef<T>& other) {
    ref_ = other.ref_;
    ref_.inc_refcnt();
    return *this;
  }

  // move constructor
  RemoteRef<T>(RemoteRef<T>&& other) {
    *this = std::move(other);
  }

  // move assignment
  RemoteRef<T>& operator=(RemoteRef<T>&& other) {
    ref_ = other.ref_;
    return *this;
  }

  // destructor
  ~RemoteRef<T>() {
    // when inuse becomes 0, the Clerk can decide whether or not to evacuate this object
    ref_.dec_refcnt();
  }

  inline T& operator*() {
    return get_ref();
  }

  inline T* operator->() {
    return &get_ref();
  }

 private:
  inline T& get_ref() {
    return *ref_;
  }

  RemotePtr<T>& ref_;
};

// Remoteable is essentially a RefCell in Rust.
// It is not a smart pointer. To call a method of its inner object, the user
// must first call `deref()` (like borrow() in Rust).
// This deref() method will return an `RemoteRef`.
// `RemoteRef` is in essential DerefScope + smart pointer (Deref trait).
// DerefScope in AIFM does not prevent the user from misuse. The user can still
// forget to add a DerefScope for a remote object, dealying the error to
// runtime.
template <typename T>
class Remoteable {
 public:
  // constructor
  template <typename... Args>
  explicit Remoteable<T>(Args &&... args) {
    ptr_ = make_remote<T>(std::forward(args)...);
  }

  Remoteable<T>() {
    ptr_ = make_remote<T>();
  }
  ~Remoteable<T>() = default;
  Remoteable<T>(const Remoteable<T>& other) = delete;
  Remoteable<T>& operator=(Remoteable<T>& other) = delete;

  Remoteable<T>& operator=(Remoteable<T>&& other) {
    ptr_ = std::move(other.ptr_);
    return *this;
  }

  Remoteable<T>(Remoteable<T>&& other) {
    *this = std::move(other);
  }

  // The most important API for this structure.
  inline RemoteRef<T> deref() {
    return RemoteRef(ptr_);
  }

 private:
  RemotePtr<T> ptr_;
};

static_assert(sizeof(Remoteable<int>) == 8);

} // namespace palkia
