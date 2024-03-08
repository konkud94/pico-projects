#include "rp2040_adc.hpp"

#include <assert.h>

#include "hardware/adc.h"

namespace {
int ChannelIdToPin(RP2040Adc::EChannelId id);
}  // namespace

RP2040Adc::RP2040Adc(std::initializer_list<EChannelId> requested_channels) {
  adc_init();
  enabled_channels_.fill(false);
  for (const auto id : requested_channels) {
    bool channel_enabled = false;
    const auto pin = ::ChannelIdToPin(id);
    if (pin >= 0) {
      adc_gpio_init(pin);
      channel_enabled = true;
    } else if (id == EChannelId::CHANNEL_TEMP_SENS) {
      adc_set_temp_sensor_enabled(true);
      channel_enabled = true;
    }
    assert(channel_enabled);
    enabled_channels_[static_cast<size_t>(id)] = channel_enabled;
  }
}
bool RP2040Adc::IsChannelAvailable(unsigned int channel) const {
  if (channel > enabled_channels_.size() - 1) {
    return false;
  }
  return enabled_channels_[channel];
}
uint16_t RP2040Adc::GetRawAdcValue(unsigned int channel) const {
  if (!IsChannelAvailable(channel)) {
    return 0;
  }
  adc_select_input(channel);
  return adc_read();
}

namespace {
int ChannelIdToPin(RP2040Adc::EChannelId id) {
  switch (id) {
    case RP2040Adc::EChannelId::CHANNEL_GPIO_26:
      return 26;
    case RP2040Adc::EChannelId::CHANNEL_GPIO_27:
      return 27;
    case RP2040Adc::EChannelId::CHANNEL_GPIO_28:
      return 28;
    case RP2040Adc::EChannelId::CHANNEL_GPIO_29:
      return 29;
    case RP2040Adc::EChannelId::CHANNEL_TEMP_SENS:
    case RP2040Adc::EChannelId::CHANNELS_COUNT:
      return -1;
  }
  return -1;
}
}  // namespace