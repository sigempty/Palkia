#include <glog/logging.h>

#include "palkia/object.h"
#include "palkia/clerk.h"

namespace palkia {

ObjectId allocate_object_id() {
  static std::atomic<ObjectId> obj_counter {0};
  return obj_counter.fetch_add(1);
}

Result Metadata::SwapOut() {
  // panic if the object is currently in use.
  CHECK(!flags.inuse) << "object is currently in use: " << ToString();
  flags.cached = false;
  return swap_ops.swap_out(&val, obj_id, storage());
}

Result Metadata::SwapIn() {
  auto rc = swap_ops.swap_in(&val, obj_id, storage());
  flags.cached = true;
  Clerk::Get()->Add(this);
  return rc;
}

}  // namespace palkia
