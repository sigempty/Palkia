#include <glog/logging.h>
#include <infiniband/verbs.h>

int main(int argc, char* argv[]) {
  ibv_fork_init();
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "Palkia service started";
  // never return;
}
