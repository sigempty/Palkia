#pragma once
#include <atomic>
#include <string>
#include <sstream>

#include "palkia/common.h"
#include "palkia/storage.h"
#include "palkia/swapable.h"

namespace palkia {

/*!
 * \brief allocate an object ID
 *
 * We use a atomic counter to allocate object ID.
 * Because we only need to ensure the ID is unique within a client process.
 * Apperantly, the entire design does not support fork at all.
 */
ObjectId allocate_object_id();

struct Metadata {
  /*! \brief object ID */
  ObjectId obj_id;
  /*! \brief the pointer to val (type-erased) */
  void* val;
  /*! \brief the size of the object, the size is treated differently for slice type */
  size_t size;
  /*! \brief swap operations function pointers */
  SwapOps swap_ops;
  /*! \brief Reference to the storage to ensure proper destruct order */
  std::shared_ptr<Storage> storage_ref;
  /*! \brief various flags */
  struct {
    /*! \brief whether the object is in local memory */
    uint64_t cached: 1;
    /*!
     * \brief a refcnt, increase only when borrow from Remoteable
     * 
     * Note that this refcnt does not represent the number of owners, so need
     * not destroy the object when it comes down to zero
     */
    uint64_t inuse: 31;
    /*! \brief unused bits */
    uint64_t _unused: 62;
  } flags;

  explicit Metadata(size_t arg_size, SwapOps arg_swap_ops)
      : obj_id(allocate_object_id()),
        val(nullptr),
        size(arg_size),
        swap_ops(arg_swap_ops),
        storage_ref(Storage::_GetSharedRef()),
        flags{1} // cached: true, inuse: 0
  {}

  inline Storage* storage() { return storage_ref.get(); }

  /*!
   * \brief force to evacuate the object to remote memory
   * 
   * If the operation fails, the value should still be there in the local
   * memory.
   */
  Result SwapOut();

  /*! \brief force to do a swap in */
  Result SwapIn();

  std::string ToString() {
    std::stringstream ss;
    ss << "{ obj_id: " << obj_id
       << ", val: " << val
       << ", size: " << size
       << ", cached: " << flags.cached
       << ", inuse: " << flags.inuse
       << " }";
    return ss.str();
  }
};

}  // namespace palkia
