#include "relay_controller_impl.hpp"

#include "../constants/constants.h"
#include "pico/stdlib.h"

RelayControllerImpl::RelayControllerImpl(const unsigned int pin) : pin_(pin) {}

void RelayControllerImpl::Activate() {
  gpio_put(pin_, Constants::Pins::kActivateRelayPinValue);
  activated_ = true;
}
void RelayControllerImpl::Deactivate() {
  gpio_put(pin_, !Constants::Pins::kActivateRelayPinValue);
  activated_ = false;
}

bool RelayControllerImpl::IsActivated() const { return activated_; };