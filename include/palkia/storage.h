#pragma once
#include <memory>

#include "palkia/common.h"

namespace palkia {

/*!
 * \brief The interface of a remote object storage.
 *
 * The remote storage could a local SSD, or a remote memory service over RDMA
 * connection.
 */
class Storage {
 public:
  /*! \brief virtual destructor */
  virtual ~Storage() {}
  /*! \brief fetch the object from storage to memory */
  virtual Result Fetch(ObjectId obj_id, void* ptr, size_t size);
  /*! \brief put the object to storage */
  virtual Result Put(ObjectId obj_id, void* ptr, size_t size);
  /*! \brief invalide the object */
  virtual Result Invalidate(ObjectId obj_id);

  /*! \return Storage singleton. */
  static Storage* Get();
  /*! \return A shared pointer to Storage singleton. */
  static const std::shared_ptr<Storage>& _GetSharedRef();
};

// TODO(cjr): Ultimately, a client should be able to choose which storage device
// to use (where to swap out an object). To enable this feature, we need more we
// need a storage manager to manage an array of Storages.

// TODO(cjr): For fault tolerance, storage also needs to be able to do snapshot and later recover from a snapshot.
// If a client quits, all the memory allocated for that client should be garbage collected / released.
// If a service quits unexpectedly, what should we do to make sure no data loss?

// TODO(cjr): For eval, we can compare RDMA remote memory storage with
// (1) local file (we have NVMe disk). The local disk storage can be further optimized by
// removing the layer of file system and directly talking to the NVMe
// (2) kernel-based swapping mechanism. The difference between (1) and (2) lies
// in the object granularity
// (3) TCP-based remote memory.

}  // namespace palkia
