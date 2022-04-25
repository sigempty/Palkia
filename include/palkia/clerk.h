#pragma once
#include "palkia/common.h"

namespace palkia {

// TODO(cjr): implement some LRU mechanism
class LRU;

class Clerk {
 public:
  static Clerk* Get();

 private:
  Clerk();

  // LRU lru;
};

}  // namespace palkia
