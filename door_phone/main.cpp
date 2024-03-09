#include <utility>

#include "ca_signal_processor/ca_signal_processor_impl.hpp"
#include "consent_provider/consent_provider_impl.hpp"
#include "constants/constants.h"
#include "hardware_clock/hardware_clock.h"
#include "initializer_list"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "relay_controller/relay_controller_impl.hpp"
#include "repeating_timer/repeating_timer.hpp"
#include "rp2040_adc/rp2040_adc.hpp"
#include "time_aware/ca_signal_observer.hpp"
#include "time_aware/cyw43_blinker.hpp"
#include "time_aware/door_electric_lock.hpp"
namespace {
void InitOutputPins() {
  for (const auto& pin : {Constants::Pins::kCaSignalMuteRelayPin,
                          Constants::Pins::kDoorLockRelayPin}) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, !Constants::Pins::kActivateRelayPinValue);
  }
}
bool InitHardware() {
  InitOutputPins();
  if (cyw43_arch_init()) {
    printf("WiFi init failed");
    return false;
  }
  return true;
}

}  // namespace

int main() {
  stdio_init_all();
  const bool hardware_init_ok = InitHardware();
  if (!hardware_init_ok) {
    printf("Failed to initialize board");
    return -1;
  }
  RelayControllerImpl relay_door(Constants::Pins::kDoorLockRelayPin);
  RelayControllerImpl relay_mute(Constants::Pins::kCaSignalMuteRelayPin);
  relay_door.Deactivate();
  /* TODO: be careful with mute activation */
  relay_mute.Activate();

  RP2040Adc adc({Constants::Adc::kCaSignalAdcChannel});
  CaSignalProcessorImpl ca_signal_processor(
      &adc, static_cast<unsigned int>(Constants::Adc::kCaSignalAdcChannel));

  RepeatingTimer timer_ca(std::move([&ca_signal_processor]() {
                            ca_signal_processor.PeriodicCallback();
                          }),
                          Constants::Intervals::kCaSignalSamplingRateMs);
  HardwareClock clock;
  DoorElectricLock door_lock(
      &clock, Constants::Intervals::kDoorElectricLockUpdateMs, &relay_door,
      Constants::Intervals::kWaitBetweenSignalAndOpenDoorMs);
  ConsentProviderImpl consent_provider;
  CaSignalObserver ca_signal_observer(
      &clock, Constants::Intervals::kProcessedCaSignalObserverRateMS,
      &ca_signal_processor, &consent_provider, &door_lock);
  Cyw43Blinker cyw43_blinker_(&clock,
                              Constants::Intervals::kCyw43ToggleLedRateMS);
  while (true) {
    door_lock.Update();
    ca_signal_observer.Update();
    cyw43_blinker_.Update();
  }
}
