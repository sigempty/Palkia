#pragma once
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#include <utility>

#include "palkia/common.h"
#include "palkia/object.h"
#include "palkia/swapable.h"

namespace palkia {

template <typename T>
class RemotePtr {
 public:
  template <typename... Args>
  static RemotePtr<T> allocate_remote(Args &&... args) {
    auto ptr = RemotePtr<T>::dangling();
    ptr.metadata = new Metadata();
    // assign a global unique ID
    // ptr.metadata->obj_id = allocate_object_id();
    // ptr.metadata->flags.cached = true;
    ptr.val = new T(std::forward(args)...);
    return std::move(ptr);
  }

  RemotePtr<T>() { }

  ~RemotePtr<T>() {
    delete val;
    val = nullptr;
    if (metadata) {
      metadata->storage()->Invalidate(metadata->obj_id);
      delete metadata;
      metadata = nullptr;
    }
  }

  RemotePtr<T>(const RemotePtr<T>& other) = delete;
  RemotePtr<T>& operator=(RemotePtr<T>& other) = delete;

  RemotePtr<T>& operator=(RemotePtr<T>&& other) {
    val = other.val;
    metadata = other.metadata;
    other.val = nullptr;
    other.metadata = nullptr;
    return *this;
  }

  RemotePtr<T>(RemotePtr<T>&& other) {
    *this = std::move(other);
  }


  inline operator bool() const {
    return val && metadata;
  }

  static inline RemotePtr<T> dangling() {
    auto ptr = RemotePtr<T>();
    return ptr;
  }

  // force to swap out to remote memory
  // if the operation fails, the value should still be there in the local memory
  inline Result swap_out() {
    // panic if the pointer is empty
    assert(this);
    metadata->flags.cached = false;
    // auto ret = metadata->storage->Put(metadata->obj_id, val, sizeof(T));
    // auto ret = val->swap_out(metadata->obj_id, metadata->storage());
    // if (ret) {
    //   return ret;
    // }
    // TODO(cjr): delete it lazily
    // delete val;
    // val = nullptr;
    // return 0;
    return SwapOut<T>(&val, metadata->obj_id, metadata->storage());
  }

  // force to do a swap in
  inline Result swap_in() {
    // if RDMA
    // return metadata->storage->Fetch(metadata->obj_id, &val, sizeof(T));
    // return val->swap_in(metadata->obj_id, metadata->storage());
    return SwapIn<T>(&val, metadata->obj_id, metadata->storage());
    // if local storage, do sth else
  }

  inline T* get() {
    if (!metadata->flags.cached) {
      // fetch the object from the remote memory
      swap_in();
    }
    return val;
  }

  inline T& operator*() {
    return *get();
  }

  inline T* operator->() {
    return get();
  }

 private:

  T* val = nullptr;
  Metadata* metadata = nullptr;
};

template <typename T, typename... Args>
RemotePtr<T> make_remote(Args &&... args) {
  return RemotePtr<T>::allocate_remote(std::forward(args)...);
}

template <typename T>
class Remoteable {
 public:
  explicit Remoteable<T>() {
    ptr = make_remote<T>();
  }

  inline T& deref() {
    return *ptr;
  }

  inline T& operator*() {
    return deref();
  }

  inline T* operator->() {
    return &deref();
  }

 private:
  RemotePtr<T> ptr;
};

} // namespace palkia
