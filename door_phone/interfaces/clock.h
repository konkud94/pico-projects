#pragma once
#include <inttypes.h>

class Clock {
 public:
  virtual ~Clock() = default;
  virtual uint64_t GetUsSinceBoot() = 0;
  virtual uint64_t GetMsSinceBoot() = 0;
};