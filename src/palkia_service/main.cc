#include <memory>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <infiniband/verbs.h>

#include "dialga/kvstore.hpp"
#include "dialga/config.hpp"

int main(int argc, char* argv[]) {
  ibv_fork_init();
  google::InitGoogleLogging(argv[0]);
  FLAGS_logtostderr = 1;
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  LOG(INFO) << "Starting Palkia service";
  auto server = std::unique_ptr<dialga::KVServer>(
      dialga::KVServer::Create(dialga::FLAGS_comm.c_str()));
  LOG(INFO) << "Palkia service initializing...";
  auto rc = server->Init();
  if (rc) {
    LOG(FATAL) << "Palkia service initialize failed...";
  }
  LOG(INFO) << "Palkia service running...";
  server->Run();
  // never return;
}
