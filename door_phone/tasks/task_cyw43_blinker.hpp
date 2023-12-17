#pragma once

#include "../interfaces/clock.h"
#include "../interfaces/task.h"

class TaskCyw43Blinker : public Task {
 public:
  TaskCyw43Blinker(Clock* clock, uint32_t interval_ms);

 protected:
  void InternalUpdate(const uint64_t current_ms) override;
};