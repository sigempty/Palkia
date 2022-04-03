#pragma once
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define UNIMPLEMENTED assert(0)

typedef uint64_t ObjectId;
typedef int Result;

class Connection {
 public:
  Result fetch(void* ptr, ObjectId obj_id, size_t size) {
    UNIMPLEMENTED;
  }
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


template <typename T>
class Remoteable {
 public:
  explicit Remoteable<T>() {
    metadata = new Metadata();
    metadata->flags.cached = true;
  }
  inline T& deref() {
    if (!metadata->flags.cached) {
      // fetch the object from the remote memory
      metadata->conn->fetch(&val, metadata->obj_id, sizeof(T));
    }
    return val;
  }

  inline T& operator*() {
    return deref();
  }

  inline T* operator->() {
    return &deref();
  }

 private:
  T val;
  Metadata* metadata;
};
