#pragma once
#include "palkia/common.h"
#include "palkia/lru.h"

namespace palkia {

#define MEMORY_WATERMARK "PALKIA_MEMORY_WATERMARK"

class Metadata;

class Clerk {
 public:
  static Clerk* Get();

  /*! \brief record new object as local, and potentially evacuate
   * past objects.
   */
  void Add(Metadata* obj);

  /*! \brief refresh the object, mark it as recently used in the table */
  void Use(ObjectId obj_id);

  /*! \brief call this when the object is been destroyed */
  void Remove(ObjectId obj_id);

 private:
  Clerk();
  ~Clerk();

  /*! \brief an upper bound given by the user or read from the system */
  size_t memory_watermark_ {0};
  /*! \brief memory in use for local remotable objects */
  size_t memory_used_ {0};
  /*! \brief objects in local memory */
  LRU cache_;
  /*! \brief for prefeching */
  LRU prefetching_; // TODO(cjr): change to MRU
};

}  // namespace palkia
