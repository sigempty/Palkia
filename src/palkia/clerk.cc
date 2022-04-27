#include <unistd.h>
#include <sys/sysinfo.h>
#include <stdlib.h>
#include <string>

#include <glog/logging.h>

#include "palkia/clerk.h"
#include "palkia/object.h"
#include "palkia/palkia.h"

namespace palkia {

size_t GetTotalFreeMemoryBytes() {
  auto free_pages = get_avphys_pages();
  auto page_size = sysconf(_SC_PAGESIZE);
  return free_pages * page_size;
}

Clerk::Clerk() {
  // initialize watermark from system
  memory_watermark_ = GetTotalFreeMemoryBytes();
  // overwrite watermark if env present
  const char* watermark = getenv(MEMORY_WATERMARK);
  if (watermark) {
    memory_watermark_ = std::stoi(watermark);
  }

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
    memory_used_ -= lucky_obj->size;
    auto rc = lucky_obj->SwapOut();
    CHECK(!rc) << "lucky_obj: " << lucky_obj->ToString();
    prefetching_.Insert(lucky_obj->obj_id, lucky_obj);
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
