#pragma once
#include <inttypes.h>

class ElectricLock {
 public:
  virtual ~ElectricLock() = default;
  virtual void Open(uint32_t open_for_ms = 3'000) = 0;
};