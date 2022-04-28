#pragma once
#include <unordered_map>
#include <deque>
#include <tuple>

#include "palkia/common.h"

namespace palkia {

class Metadata;

// The larger the less probability to be evacuated.
using Score = int64_t;
using Timestamp = size_t;

// The LRU should prefer to evacuate large and least recently used item.
class LRU {
 public:
  /*! \brief insert an item */
  void Insert(ObjectId key, Metadata* obj);

  /*! \brief update the timestamp of an item */
  void Touch(ObjectId key);

  /*! \brief pop the least recently used item */
  Metadata* Pop();

  /*! \brief remove a particular item */
  Metadata* Remove(ObjectId key);

  inline Score Evaulate(std::tuple<Timestamp, Metadata*> entry) {
    // TODO(cjr): Currently we only consider the timestamp.
    auto ts = std::get<0>(entry);
    // auto metadata = std::get<1>(entry);
    return ts;
  }

  // All the objects are destroyed by the user, not here.
  Timestamp cur_ts_;
  std::unordered_map<ObjectId, std::tuple<Timestamp, Metadata*>> table_;
  std::deque<std::tuple<ObjectId, Timestamp>> queue_;
};

}  // namespace palkia
