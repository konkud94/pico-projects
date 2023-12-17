#pragma once
#include <inttypes.h>

#include "../interfaces/clock.h"

class HardwareClock : public Clock {
 public:
  uint64_t GetUsSinceBoot() override;
  uint64_t GetMsSinceBoot() override;
};