#include "repeating_timer.hpp"

RepeatingTimer::RepeatingTimer(std::function<void()>&& callback, uint32_t ms)
    : callback_(std::move(callback)) {
  add_repeating_timer_ms(-static_cast<int32_t>(ms), TimerCallback, this,
                         &timer_);
}

bool RepeatingTimer::TimerCallback(struct repeating_timer* t) {
  auto* timer_instance = reinterpret_cast<RepeatingTimer*>(t->user_data);
  timer_instance->callback_();
  return true;
}
