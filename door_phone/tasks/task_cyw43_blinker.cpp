#include "task_cyw43_blinker.hpp"

#include "pico/cyw43_arch.h"

TaskCyw43Blinker::TaskCyw43Blinker(Clock* clock, uint32_t interval_ms)
    : Task(clock, interval_ms) {}

void TaskCyw43Blinker::InternalUpdate(const uint64_t current_ms) {
  (void)current_ms;
  const bool pin_val = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !pin_val);
}
