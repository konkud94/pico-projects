#pragma once

#include <inttypes.h>
#include <stdlib.h>

#include <array>
#include <initializer_list>

#include "../interfaces/adc_reader.h"
/*
    TODO: it is a singleton actually
*/
class RP2040Adc : public AdcReader {
 public:
  enum class EChannelId : unsigned int {
    CHANNEL_GPIO_26 = 0,
    CHANNEL_GPIO_27 = 1,
    CHANNEL_GPIO_28 = 2,
    CHANNEL_GPIO_29 = 3,
    CHANNEL_TEMP_SENS = 4,

    /* no adding beoynd this point */
    CHANNELS_COUNT,
  };
  RP2040Adc(std::initializer_list<EChannelId> requested_channels);
  bool IsChannelAvailable(unsigned int channel) const override;
  uint16_t GetRawAdcValue(unsigned int channel) const override;

 private:
  std::array<bool, static_cast<size_t>(EChannelId::CHANNELS_COUNT)>
      enabled_channels_;
};