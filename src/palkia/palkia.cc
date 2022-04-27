#include <gflags/gflags.h>
#include <glog/logging.h>
#include <infiniband/verbs.h>

#include "palkia/palkia.h"

namespace palkia {

void Init(int* argc, char** argv[]) {
  ibv_fork_init();
  gflags::ParseCommandLineFlags(argc, argv, true);
  google::InitGoogleLogging(*argv[0]);
  Storage::Get();
  LOG(INFO) << "Palkia client initialized";
}

}  // namespace palkia
