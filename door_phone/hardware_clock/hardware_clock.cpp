#include "hardware_clock.h"

#include "pico/time.h"

uint64_t HardwareClock::GetUsSinceBoot() {
  const auto absolute = get_absolute_time();
  const uint64_t us = to_us_since_boot(absolute);
  return us;
}
uint64_t HardwareClock::GetMsSinceBoot() {
  return GetUsSinceBoot() / static_cast<uint64_t>(1000);
}
