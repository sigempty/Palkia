#include <glog/logging.h>

#include <iostream>
#include <vector>

#include <palkia/palkia.h>
#include <prism/utils.h>

#include "./common.h"

DEFINE_int32(num_objects, 50, "Number of objects");

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  palkia::Init(&argc, &argv);

  int n = FLAGS_num_objects;
  std::vector<palkia::Remoteable<Object>> vec;
  for (int i = 0; i < n; i++) {
    auto obj = palkia::Remoteable<Object>();
    {
      auto ref = obj.deref();
      ref->buf[0] = '\0';
    }
    vec.push_back(std::move(obj));
  }
  LOG(INFO) << "execute done";

  auto start = Now64();
  for (int k = 0; k < 5; k++) {
    for (int i = 0; i < n; i++) {
      auto ref = vec[i].deref();
      // std::array<uint8_t, MSG_SIZE>& buf = ref->buf;
      for (int j = 0; j < MSG_SIZE; j++) {
        ref->buf[j] = (char)(j + i);
      }
    }
  }

  auto end = Now64();
  LOG(INFO) << "exit...  result = " << (end - start) / 1e3 << " us";
  return 0;
}
