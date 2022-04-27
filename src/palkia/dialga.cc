#include <atomic>
#include <x86intrin.h>

#include <gflags/gflags.h>
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
  kvstore_ = std::unique_ptr<dialga::KVStore>(
      dialga::KVStore::Create(dialga::FLAGS_comm.c_str()));
  if (!kvstore_) {
    LOG(ERROR) << "Dialga KVStore (comm=" << dialga::FLAGS_comm.c_str()
               << ") creation failed";
    return 1;
  }
  auto rc = kvstore_->Init();
  if (rc) {
    LOG(ERROR) << "Dialga KVStore initialization failed";
    return 2;
  }
  return 0;
}

void HexDump(ObjectId obj_id, void* ptr, size_t size) {
  if (size > 32) {
    size = 32;
  }
  LOG(INFO) << "obj_id: " << obj_id;
  char buf[128];
  size_t off = 0;
  for (size_t i = 0; i < size; i++) {
    off += snprintf(buf + off, 128 - off, "%02hhx ", ((uint8_t*)ptr)[i]);
  }
  LOG(INFO) << "addr: " << ptr << ", content: " << buf;
}

Result DialgaAdapter::Fetch(ObjectId obj_id, void* ptr, size_t size) {
  CHECK(ptr);
  std::vector<dialga::Key> keys {obj_id};
  std::vector<dialga::Value*> values{
      new dialga::Value(reinterpret_cast<uint64_t>(ptr), size)};
  volatile std::atomic<bool> ready{false};
  auto rc = kvstore_->Get(keys, values, [&ready]() { ready.store(true); });
  if (rc) {
    LOG(ERROR) << "Get failed for obj: " << obj_id << ", ptr: " << ptr << ", size: " << size;
    return rc;
  }
  while (!ready.load()) _mm_pause();
  CHECK_EQ(ptr, (void*)values[0]->addr_);
  // LOG(INFO) << "Fetch";
  // HexDump(obj_id, ptr, size);
  delete values[0];
  return 0;
}

Result DialgaAdapter::Put(ObjectId obj_id, void* ptr, size_t size) {
  CHECK(ptr);
  std::vector<dialga::Key> keys {obj_id};
  std::vector<dialga::Value> values{
      dialga::Value(reinterpret_cast<uint64_t>(ptr), size)};
  // LOG(INFO) << "Put";
  // HexDump(obj_id, ptr, size);
  volatile std::atomic<bool> ready{false};
  auto rc = kvstore_->Put(keys, values, [&ready]() { ready.store(true); });
  if (rc) {
    LOG(ERROR) << "Put failed for obj: " << obj_id << ", ptr: " << ptr << ", size: " << size;
    return rc;
  }
  // TODO(cjr): I don't like this code.
  while (!ready.load()) _mm_pause();
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
