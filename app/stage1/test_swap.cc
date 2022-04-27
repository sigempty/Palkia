#include <stdio.h>
#include <iostream>
#include <array>
#include <vector>

#include <glog/logging.h>

#include <palkia/palkia.h>

struct Page {
  std::array<uint8_t, 4096> page;
};

int main(int argc, char* argv[]) {
  palkia::Init(&argc, &argv);
  std::vector<palkia::Remoteable<Page>> pages;
  for (size_t i = 0; i < 1000; i++) {
    auto rp = palkia::Remoteable<Page>();
    pages.push_back(std::move(rp));
  }

  uint8_t byte = 0;
  for (auto& rp : pages) {
    auto ref = rp.deref();
    memset(ref->page.data(), byte, ref->page.size());
    byte += 1; // let it overflow
  }

  byte = 0;
  for (auto& rp : pages) {
    auto ref = rp.deref();
    size_t sum = 0;
    for (auto it = ref->page.begin(); it != ref->page.end(); it++) {
      sum += static_cast<size_t>(*it);
    }
    CHECK_EQ(sum, ref->page.size() * byte);
    byte += 1;
  }
}
