#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string>
#include <fstream>

#include <glog/logging.h>

#include "palkia/clerk.h"
#include "palkia/object.h"
#include "palkia/palkia.h"
#include "prism/utils.h"

namespace palkia {

size_t GetTotalFreeMemoryBytes() {
  std::ifstream fin("/proc/self/cgroup");
  std::string line;
  std::string memory_cgroup;
  const char* kMemoryPattern = "10:memory:";
  const size_t kMemPatLen = strlen(kMemoryPattern);
  while (std::getline(fin, line)) {
    if (!strncmp(line.c_str(), kMemoryPattern, kMemPatLen)) {
      memory_cgroup = line.substr(kMemPatLen);
    }
  }
  auto mlimit_path = prism::FormatString(
      "/sys/fs/cgroup/memory%s/memory.limit_in_bytes", memory_cgroup.c_str());
  std::ifstream mlimit_fin(mlimit_path);
  size_t mlimit;
  mlimit_fin >> mlimit;
  return mlimit;
}

Clerk::Clerk() {
  // initialize watermark from system
  auto free_memory = GetTotalFreeMemoryBytes();
  memory_watermark_ = (size_t)(free_memory * 0.8);
  // overwrite watermark if env present
  const char* watermark = getenv(MEMORY_WATERMARK);
  if (watermark) {
    memory_watermark_ = std::stoll(watermark);
  }

  LOG(INFO) << "Total available memory: " << free_memory << " bytes";
  LOG(INFO) << "Memory watermark: " << memory_watermark_ << " bytes";
}

Clerk::~Clerk() {
  // destroy all objects in both tables
  // All the objects are destroyed by the user, not here.
}

Clerk* Clerk::Get() {
  static Clerk inst;
  return &inst;
}

void Clerk::Add(Metadata* obj) {
  auto obj_id = obj->obj_id;
  memory_used_ += obj->size;
  cache_.Insert(obj_id, obj);
  if (memory_used_ >= memory_watermark_) {
    // find some lucky baby and swap it out
    auto lucky_obj = cache_.Pop();
    if (lucky_obj) {
      CHECK(lucky_obj->flags.cached) << lucky_obj->ToString();
      auto rc = lucky_obj->SwapOut();
      CHECK(!rc) << "evacuate failed lucky_obj: " << lucky_obj->ToString();
      memory_used_ -= lucky_obj->size;
      prefetching_.Insert(lucky_obj->obj_id, lucky_obj);
    } else {
      LOG(WARNING) << "nothing to pop, memory_used_: " << memory_used_
                   << ", watermark: " << memory_watermark_;
    }
  }
}

void Clerk::Use(ObjectId obj_id) {
  cache_.Touch(obj_id);
}

void Clerk::Remove(ObjectId obj_id) {
  // try to search for the obj in both table
  auto obj = cache_.Remove(obj_id);
  if (!obj) {
    obj = prefetching_.Remove(obj_id);
  }
  CHECK(obj) << "obj not found in any table, obj_id: " << obj_id;
  memory_used_ -= obj->size;
}

}  // namespace palkia
