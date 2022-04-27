#include <atomic>
#include <x86intrin.h>

#include <glog/logging.h>
#include "./dialga.h"

namespace palkia {

DialgaAdapter::DialgaAdapter() {
  auto rc = Init();
  if (rc) {
    LOG(FATAL) << "DialgaAdapter failed to initialize";
  }
}

Result DialgaAdapter::Init() {
  kvstore_ = std::unique_ptr<dialga::KVStore>(dialga::KVStore::Create("rdma"));
  if (!kvstore_) {
    LOG(ERROR) << "Dialga KVStore (comm=rdma) creation failed";
    return 1;
  }
  auto rc = kvstore_->Init();
  if (rc) {
    LOG(ERROR) << "Dialga KVStore initialization failed";
    return 2;
  }
  return 0;
}

Result DialgaAdapter::Fetch(ObjectId obj_id, void* ptr, size_t size) {
  // CHECK(ptr);
  std::vector<dialga::Key> keys {obj_id};
  std::vector<dialga::Value*> values{
      new dialga::Value(reinterpret_cast<uint64_t>(ptr), size)};
  volatile std::atomic<bool> ready{false};
  auto rc = kvstore_->Get(keys, values, [&ready]() {
                  ready.store(true);
                });
  if (rc) {
    LOG(ERROR) << "Get failed for obj: " << obj_id << ", ptr: " << ptr << ", size: " << size;
    return rc;
  }
  while (!ready.load()) _mm_pause();
  *(void**)ptr = (void*)values[0]->addr_;
  CHECK(*(void**)ptr);
  return 0;
}

Result DialgaAdapter::Put(ObjectId obj_id, void* ptr, size_t size) {
  CHECK(ptr);
  std::vector<dialga::Key> keys {obj_id};
  std::vector<dialga::Value> values{
      dialga::Value(reinterpret_cast<uint64_t>(ptr), size)};
  auto rc = kvstore_->Put(keys, values);
  if (rc) {
    LOG(ERROR) << "Put failed for obj: " << obj_id << ", ptr: " << ptr << ", size: " << size;
    return rc;
  }
  return 0;
}

Result DialgaAdapter::Invalidate(ObjectId obj_id) {
  std::vector<dialga::Key> keys {obj_id};
  auto rc = kvstore_->Delete(keys);
  if (rc) {
    LOG(ERROR) << "Delete failed for obj: " << obj_id;
    return rc;
  }
  return 0;
}

}  // namespace palkia
