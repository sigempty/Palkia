#pragma once
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include <gflags/gflags.h>

#include <array>

inline uint64_t Now64() {
  struct timespec tv;
  int res = clock_gettime(CLOCK_REALTIME, &tv);
  return (uint64_t)tv.tv_sec * 1000000000llu + (uint64_t)tv.tv_nsec;
}


#define MSG_SIZE (1048576)

struct Object {
  std::array<uint8_t, MSG_SIZE> buf;
};
