#pragma once

#include "../interfaces/clock.h"
#include "../interfaces/time_aware.h"

class Cyw43Blinker : public TimeAware {
 public:
  Cyw43Blinker(Clock* clock, uint32_t interval_ms);

 protected:
  void InternalUpdate(const uint64_t current_ms) override;
};