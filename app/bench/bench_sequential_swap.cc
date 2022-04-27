#include <glog/logging.h>

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "./common.h"


DEFINE_int32(num_objects, 50, "Number of objects");

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  int n = FLAGS_num_objects;
  std::vector<std::unique_ptr<Object>> vec;
  for (int i = 0; i < n; i++) {
    auto obj = std::make_unique<Object>();
    obj->buf[0] = '\0';
    vec.push_back(std::move(obj));
  }
  LOG(INFO) << "execute done";

  auto start = Now64();
  for (int k = 0; k < 5; k++) {
    for (int i = 0; i < n; i++) {
      auto ref = vec[i].get();
      for (int j = 0; j < MSG_SIZE; j++) {
        ref->buf[j] = (char)(j + i);
      }
    }
  }

  auto end = Now64();
  LOG(INFO) << "exit...  result = " << (end - start) / 1e3 << " us";
  return 0;
}
