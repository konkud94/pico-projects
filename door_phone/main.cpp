#include "ca_signal_processor/ca_signal_processor.hpp"
#include "initializer_list"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "rp2040_adc/rp2040_adc.hpp"
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
uint32_t GetMsSinceReboot() {
  const auto absolute = get_absolute_time();
  return to_ms_since_boot(absolute);
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

bool PeriodicCallbackToggleLed(struct repeating_timer* t) {
  (void)t;
  const bool pin_val = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !pin_val);
  return true;
}
int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("WiFi init failed");
    return -1;
  }
  InitOutputPins();
  const auto adc_ca_channel = RP2040Adc::EChannelId::CHANNEL_GPIO_28;
  RP2040Adc adc({adc_ca_channel});
  CaSignalProcessor ca_signal_processor(
      &adc, static_cast<unsigned int>(adc_ca_channel));
  ca_signal_processor_ptr = &ca_signal_processor;
  struct repeating_timer timer_ca;
  const int32_t ca_signal_sample_rate_ms = 5;
  add_repeating_timer_ms(-ca_signal_sample_rate_ms,
                         PeriodicCallbackCaSignalProcessor, NULL, &timer_ca);
  //   struct repeating_timer timer_led;
  //   const int32_t toggle_led_rate_ms = 500;
  //   add_repeating_timer_ms(-toggle_led_rate_ms, PeriodicCallbackToggleLed,
  //   NULL,
  //                          &timer_led);
  //   printf("repeating timer led added \n");
  while (true) {
    ;
  }
}
