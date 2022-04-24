#include "palkia/storage.h"
// #include "./dialga.h"

namespace palkia {

Result Storage::Fetch(ObjectId obj_id, void* ptr, size_t size) {
  UNIMPLEMENTED;
}

Result Storage::Put(ObjectId obj_id, void* ptr, size_t size) {
  UNIMPLEMENTED;
}

Result Storage::Invalidate(ObjectId obj_id) {
  return 0;
}

const std::shared_ptr<Storage>& Storage::_GetSharedRef() {
  UNIMPLEMENTED;
  // static std::shared_ptr<Storage> inst(new DialgaAdapter());
  // return inst;
}

Storage* Storage::Get() {
  static Storage* ptr = _GetSharedRef().get();
  return ptr;
}

}  // namespace palkia
