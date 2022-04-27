#include <glog/logging.h>

#include "palkia/lru.h"
#include "palkia/object.h"

namespace palkia {

void LRU::Insert(ObjectId key, Metadata* obj) {
  cur_ts_ += 1;
  queue_.push_back(std::make_tuple(key, cur_ts_));
  // no need to panic if the object is already in the cache, it is fine
  table_[key] = std::make_tuple(cur_ts_, obj);
}

void LRU::Touch(ObjectId key) {
  cur_ts_ += 1;
  auto it = table_.find(key);
  if (it != table_.end()) {
    std::get<0>(it->second) = cur_ts_;
    queue_.push_back(std::make_tuple(key, cur_ts_));
  } else {
    CHECK(0) << "key not found in LRU cache: " << key;
  }
}

Metadata* LRU::Pop() {
  Metadata* ret = nullptr;
  while (!queue_.empty()) {
    auto [obj_id, ts] = queue_.front();
    auto it = table_.find(obj_id);
    if (it != table_.end()) {
      auto [new_ts, obj] = it->second;
      if (ts == new_ts && !obj->flags.inuse) {
        ret = obj;
        break;
      }
    }
  }
  return ret;
}

Metadata* LRU::Remove(ObjectId key) {
  Metadata* ret = nullptr;
  auto it = table_.find(key);
  if (it != table_.end()) {
    ret = std::get<1>(it->second);
    table_.erase(it);
  }
  return ret;
}

}  // namespace palkia
