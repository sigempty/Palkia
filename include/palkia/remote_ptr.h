#pragma once
#include <glog/logging.h>

#include "palkia/common.h"
#include "palkia/clerk.h"
#include "palkia/object.h"
#include "palkia/swapable.h"

namespace palkia {

template <typename T>
class Remoteable;

// template <typename T>
// class RemoteSlice {
//  public:
//   static RemotePtr<T> allocate_remote_n(size_t n, T val) {
//     auto ptr = RemoteSlice<T>::dangling();
//     ptr.metadata_ = new Metadata(sizeof(T) * n);
//     ptr.val() = new T [n];
//     for (int i = 0; i < n; i++) {
//       ptr.val()[i] = val;
//     }
//     Clerk::Get()->Add(ptr);
//     return std::move(ptr);
//   }
// };


// The user should not directly use this abstraction.
template <typename T>
class RemotePtr {
 public:
  friend class Clerk;
  friend class Remoteable<T>;

  template <typename... Args>
  static RemotePtr<T> allocate_remote(Args &&... args) {
    auto ptr = RemotePtr<T>::dangling();
    SwapOps swap_ops((T*)nullptr);
    ptr.metadata_ = new Metadata(sizeof(T), swap_ops);
    ptr.val() = new T(std::forward(args)...);
    Clerk::Get()->Add(ptr.metadata_);
    return std::move(ptr);
  }

  RemotePtr<T>() { }

  ~RemotePtr<T>() {
    if (metadata_) {
      // TODO(cjr): the address may come from RdmaManager after an swap in/out
      // round trip, call delete here will fail in this case.
      delete val();
      val() = nullptr;

      Clerk::Get()->Remove(obj_id());
      metadata_->storage()->Invalidate(obj_id());
      delete metadata_;
      metadata_ = nullptr;
    }
  }

  RemotePtr<T>(const RemotePtr<T>& other) = delete;
  RemotePtr<T>& operator=(RemotePtr<T>& other) = delete;

  RemotePtr<T>& operator=(RemotePtr<T>&& other) {
    metadata_ = other.metadata_;
    other.metadata_ = nullptr;
    return *this;
  }

  RemotePtr<T>(RemotePtr<T>&& other) {
    *this = std::move(other);
  }

  inline operator bool() const {
    return metadata_ && val();
  }

  static inline RemotePtr<T> dangling() {
    auto ptr = RemotePtr<T>();
    return ptr;
  }

  /*!
   * \brief increase the inuse reference count
   *
   * This function should not be used by the user directly.
   */
  inline void inc_refcnt() {
    // TODO(cjr): A correct implementation should check for integer overflow
    metadata_->flags.inuse += 1;
  }

  /*!
   * \brief decrease the inuse reference count
   *
   * This function should not be used by the user directly.
   */
  inline void dec_refcnt() {
    // TODO(cjr): A correct implementation should check for integer overflow
    metadata_->flags.inuse -= 1;
  }

  // RefCell<T> obj;
  // {
  //   <s>DerefScope s(obj);</s>
  //   auto ref = obj->borrow(); // Ref RefMut
  //   ref->foo();
  // }

  inline T* get() {
    if (!metadata_->flags.cached) {
      // fetch the object from the remote memory
      metadata_->SwapIn();
    } else {
      Clerk::Get()->Use(obj_id());
    }
    return val();
  }

  inline T& operator*() {
    return *get();
  }

  inline T* operator->() {
    return get();
  }

 private:
  inline ObjectId obj_id() { return CHECK_NOTNULL(metadata_)->obj_id; }
  // inline T*& val() { return reinterpret_cast<T*>(CHECK_NOTNULL(metadata_)->val); }
  // inline T*& val() { return (T*&)(CHECK_NOTNULL(metadata_)->val); }
  inline T*& val() {
    // TODO(cjr): double-check the correctness
    void*& v = CHECK_NOTNULL(metadata_)->val;
    return (T*&)v;
  }

  Metadata* metadata_ = nullptr;
};

template <typename T, typename... Args>
RemotePtr<T> make_remote(Args &&... args) {
  return RemotePtr<T>::allocate_remote(std::forward(args)...);
}


}  // namespace palkia
