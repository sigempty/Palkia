#pragma once
#include <type_traits>

#include "palkia/common.h"
#include "palkia/storage.h"

namespace palkia {
/*!
 * \brief force to swap out to remote memory
 *
 * If the operation fails, the value should still be there in the local memory
 */
template <typename T>
Result SwapOut(T** val, ObjectId obj_id, Storage* storage);

/*! \brief force to do a swap in */
template <typename T>
Result SwapIn(T** val, ObjectId obj_id, Storage* storage);

// IsPod
template <typename T,
          class = typename std::enable_if_t<std::is_pod_v<T>>>
struct PodType {
  typedef T value_type;
  T data;
};

template <typename T>
Result SwapOut(typename PodType<T>::value_type** val, ObjectId obj_id,
               Storage* storage) {
  auto ret = storage->Put(obj_id, *val, sizeof(T));
  if (ret) {
    return ret;
  }
  delete *val;
  *val = nullptr;
  return 0;
}

template <typename T>
Result SwapIn(typename PodType<T>::value_type** val, ObjectId obj_id,
              Storage* storage) {
  // allocate if val is nullptr
  // if (!*val) {
  //   *val = new T();
  // }
  // Dialga will allocate for us.
  auto ret = storage->Fetch(obj_id, *val, sizeof(T));
  return ret;
}

// class ISwap {
//  public:
//   /*!
//    * \brief force to swap out to remote memory
//    *
//    * If the operation fails, the value should still be there in the local memory
//    */
//   virtual Result swap_out(ObjectId obj_id, Storage* storage);
//   /*! \brief force to do a swap in */
//   virtual Result swap_in(ObjectId obj_id, Storage* storage);
// };

// template <typename T>
// class Swapable : public ISwap {
//  public:
//   static_assert(std::is_pod<T>::value, "T must be POD"); // maybe is_trivial?
//   typedef T value_type;
// 
//   Result swap_out(ObjectId obj_id, Storage* storage) override {
//   }
// 
//   Result swap_in(ObjectId obj_id, Storage* storage) override {
//   }
// };

// static_assert(sizeof(Swapable<int>) == 8, "T must be POD"); // maybe is_trivial?

// The first challenge here is that only serializable data types (no
// matter how many memory segments it can be serialized into) can be safely
// swapped out/in.
//
// A data type has internal pointers is swapable only in certain special
// circumstances.
//
// (1) When the internal pointers all have __longer lifetime__ then
// the data type, the data type is then safe to swap.
//
// (2) __Non-self-referential__. If a POD struct is self-referential,
// after a swap out/in, the internal pointers become invalid.
// C++ just simply cannot identify this case.
//
// We limit the use case to data types that only has a single memory block.
// Meanwhile, the user is responsible for precluding invalid nested pointers.

}  // namespace palkia
