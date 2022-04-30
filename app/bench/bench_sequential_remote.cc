#include <glog/logging.h>

#include <iostream>
#include <vector>
#include <thread>
#include <numeric>

#include <palkia/palkia.h>
#include <prism/utils.h>

#include "./common.h"

DEFINE_int32(num_objects, 50, "Number of objects");
DEFINE_int32(num_threads, 1, "Number of threads");

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = 1;
  palkia::Init(&argc, &argv);

  int n = FLAGS_num_objects;
  int m = FLAGS_num_objects;
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

    LOG(INFO) << "exit...  result = "
              << prism::FormatString("%.3f", (end - start) / 1e6) << " ms";
    return 0;
}
