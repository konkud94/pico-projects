#pragma once

#include <inttypes.h>

#include <array>
#include <functional>

#include "pico/time.h"

/* currently uses ONLY default alarm pool */
class RepeatingTimer {
 public:
  RepeatingTimer(std::function<void()>&& callback, uint32_t ms);

  static bool TimerCallback(struct repeating_timer* t);

 private:
  std::function<void()> callback_;
  struct repeating_timer timer_;
};