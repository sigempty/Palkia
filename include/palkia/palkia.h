#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#include <utility>

#define UNIMPLEMENTED assert(0)

typedef uint64_t ObjectId;
typedef int Result;

class Connection {
 public:
  Result fetch(void* ptr, ObjectId obj_id, size_t size);
  Result put(void* ptr, ObjectId obj_id, size_t size);
  Result invalidate(ObjectId obj_id);
 private:
  struct ibv_qp* qp_;
};

struct Metadata {
  ObjectId obj_id;
  Connection* conn;
  struct {
    uint64_t cached: 1;
    uint64_t unused: 63;
  } flags;
};

// struct Metadata {
//   union {
//     struct {
//       uint64_t cached: 1;
//       uint64_t unused: 15;
//       uint64_t __never_use: 48;
//     };
//     Connection* conn;
//   };
// };

/*!
 * struct Foo {
 *   int a;
 *   int b;
 * };
 * 
 * auto a = Remoteable<Foo>::Create();
 * a->b;
*/

inline ObjectId allocate_object_id() {
  return 0;
}

template <typename T>
class RemotePtr {
 public:
  template <typename... Args>
  static RemotePtr<T> allocate_remote(Args &&... args) {
    auto ptr = RemotePtr<T>::dangling();
    ptr.metadata = new Metadata();
    // assign a global unique ID
    ptr.metadata->obj_id = allocate_object_id();
    ptr.metadata->flags.cached = true;
    ptr.val = new T(std::forward(args)...);
    return std::move(ptr);
  }

  RemotePtr<T>() { }

  ~RemotePtr<T>() {
    delete val;
    val = nullptr;
    if (metadata) {
      metadata->conn->invalidate(metadata->obj_id);
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
    auto ret = metadata->conn->put(val, metadata->obj_id, sizeof(T));
    if (ret) {
      return ret;
    }
    // TODO(cjr): delete it lazily
    delete val;
    val = nullptr;
    return 0;
  }

  // force to do a swap in
  inline Result swap_in() {
    // if RDMA
    return metadata->conn->fetch(&val, metadata->obj_id, sizeof(T));
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
