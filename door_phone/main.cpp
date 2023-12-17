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
  /*
   0 - lock relay
   1 - CA mute relay
  */
  for (const auto& pin : {0, 1}) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, true);
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
  const auto adc_ca_channel = RP2040Adc::EChannelId::CHANNEL_GPIO_28;
  RP2040Adc adc({adc_ca_channel});
  CaSignalProcessor ca_signal_processor(
      &adc, static_cast<unsigned int>(adc_ca_channel));
  ca_signal_processor_ptr = &ca_signal_processor;
  struct repeating_timer timer_ca;
  const int32_t ca_signal_sample_rate_ms = 5;
  add_repeating_timer_ms(-ca_signal_sample_rate_ms,
                         PeriodicCallbackCaSignalProcessor, NULL, &timer_ca);
  HardwareClock clock;
  TaskCyw43Blinker cyw43_blinker_task(
      &clock, Constants::Intervals::kCyw43ToggleLedRateMS);
  while (true) {
    cyw43_blinker_task.Update();
  }
}
