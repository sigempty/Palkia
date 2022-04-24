#include "palkia/object.h"

namespace palkia {

ObjectId allocate_object_id() {
  static std::atomic<ObjectId> obj_counter {0};
  return obj_counter.fetch_add(1);
}

}  // namespace palkia
