#include "ca_signal_processor/ca_signal_processor.hpp"
#include "constants/constants.h"
#include "hardware_clock/hardware_clock.h"
#include "initializer_list"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "rp2040_adc/rp2040_adc.hpp"
#include "tasks/task_cyw43_blinker.hpp"
namespace {
void InitOutputPins() {
  for (const auto& pin : {Constants::Pins::kCaSignalMuteRelayPin,
                          Constants::Pins::kDoorLockRelayPin}) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, !Constants::Pins::kActivateRelayPinValue);
  }
}
}  // namespace

CaSignalProcessor* ca_signal_processor_ptr = nullptr;
bool PeriodicCallbackCaSignalProcessor(struct repeating_timer* t) {
  (void)t;
  if (ca_signal_processor_ptr != nullptr) {
    ca_signal_processor_ptr->PeriodicCallback();
  }
  return true;
}

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("WiFi init failed");
    return -1;
  }
  ::InitOutputPins();
  RP2040Adc adc({Constants::Adc::kCaSignalAdcChannel});
  CaSignalProcessor ca_signal_processor(
      &adc, static_cast<unsigned int>(Constants::Adc::kCaSignalAdcChannel));
  ca_signal_processor_ptr = &ca_signal_processor;
  struct repeating_timer timer_ca;
  add_repeating_timer_ms(
      -static_cast<int32_t>(Constants::Intervals::kCaSignalSamplingRateMs),
      PeriodicCallbackCaSignalProcessor, NULL, &timer_ca);
  HardwareClock clock;
  TaskCyw43Blinker cyw43_blinker_task(
      &clock, Constants::Intervals::kCyw43ToggleLedRateMS);
  while (true) {
    cyw43_blinker_task.Update();
  }
}
