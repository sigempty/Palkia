#pragma once
#include "palkia/storage.h"
#include "dialga/kvstore.hpp"

namespace palkia {

class DialgaAdapter : public Storage {
 public:
  DialgaAdapter();
  Result Fetch(ObjectId obj_id, void* ptr, size_t size) override;
  Result Put(ObjectId obj_id, void* ptr, size_t size) override;
  Result Invalidate(ObjectId obj_id) override;

  Result Init();
 private:
  std::unique_ptr<dialga::KVStore> kvstore_;
};

}  // namespace palkia
