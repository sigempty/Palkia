#include <glog/logging.h>
#include <infiniband/verbs.h>

int main(int argc, char* argv[]) {
  ibv_fork_init();
  InitGoogleLogging(argc, argv);
  // never return;
}
