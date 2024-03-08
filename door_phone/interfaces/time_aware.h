#pragma once
#include <inttypes.h>

#include "clock.h"
class TimeAware {
 public:
  TimeAware(Clock* clock, const uint32_t interval_ms)
      : clock_(clock), interval_ms_(interval_ms) {}
  virtual ~TimeAware() = default;
  void Update() {
    const uint64_t current_ms = clock_->GetMsSinceBoot();
    if (first_update_ || current_ms >= next_update_at_ms) {
      const uint64_t base_point_ms =
          first_update_ ? current_ms : next_update_at_ms;
      next_update_at_ms = base_point_ms + interval_ms_;
      InternalUpdate(current_ms);
      first_update_ = false;
    }
  }

 private:
  Clock* const clock_;
  bool first_update_ = true;

 protected:
  virtual void InternalUpdate(const uint64_t current_ms) = 0;
  const uint32_t interval_ms_;
  uint64_t next_update_at_ms = 0;
};