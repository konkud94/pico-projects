#include <utility>

#include "ca_signal_processor/ca_signal_processor_impl.hpp"
#include "constants/constants.h"
#include "hardware_clock/hardware_clock.h"
#include "initializer_list"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "repeating_timer/repeating_timer.hpp"
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

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("WiFi init failed");
    return -1;
  }
  ::InitOutputPins();
  RP2040Adc adc({Constants::Adc::kCaSignalAdcChannel});
  CaSignalProcessorImpl ca_signal_processor(
      &adc, static_cast<unsigned int>(Constants::Adc::kCaSignalAdcChannel));

  RepeatingTimer timer_ca(std::move([&ca_signal_processor]() {
                            ca_signal_processor.PeriodicCallback();
                          }),
                          Constants::Intervals::kCaSignalSamplingRateMs);
  HardwareClock clock;
  TaskCyw43Blinker cyw43_blinker_task(
      &clock, Constants::Intervals::kCyw43ToggleLedRateMS);
  while (true) {
    cyw43_blinker_task.Update();
  }
}
