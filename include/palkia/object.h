#pragma once
#include <atomic>

#include "palkia/common.h"
#include "palkia/storage.h"

namespace palkia {

/*!
 * \brief allocate an object ID
 *
 * We use a atomic counter to allocate object ID.
 * Because we only need to ensure the ID is unique within a client process.
 * Apperantly, the entire design does not support fork at all.
 */
ObjectId allocate_object_id();

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
struct Metadata {
  /*! \brief object ID */
  ObjectId obj_id;
  /*! \brief Reference to the storage to ensure proper destruct order */
  std::shared_ptr<Storage> storage_ref;
  /*! \brief various flags */
  struct {
    uint64_t cached: 1;
    uint64_t unused: 63;
  } flags;

  explicit Metadata()
      : obj_id(allocate_object_id()),
        storage_ref(Storage::_GetSharedRef()),
        flags{1} {}

  inline Storage* storage() { return storage_ref.get(); }
};

}  // namespace palkia
