#pragma once

#include <inttypes.h>

#include "../rp2040_adc/rp2040_adc.hpp"

struct Constants {
  struct Adc {
    static constexpr auto kCaSignalAdcChannel =
        RP2040Adc::EChannelId::CHANNEL_GPIO_28;
  };
  struct Pins {
    static constexpr unsigned int kDoorLockRelayPin = 0;
    static constexpr unsigned int kCaSignalMuteRelayPin = 1;
    /*
        CaSignalMuteRelay active - mute ON
        DoorLockRelay active - door OPEN
    */
    static constexpr bool kActivateRelayPinValue = false;
  };
  struct Intervals {
    static constexpr uint32_t kCaSignalSamplingRateMs = 5;
    static constexpr uint32_t kCyw43ToggleLedRateMS = 500;
  };
};