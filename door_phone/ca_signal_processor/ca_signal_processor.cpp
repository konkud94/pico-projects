#include "ca_signal_processor.hpp"

#include <assert.h>

#include <algorithm>
CaSignalProcessor::CaSignalProcessor(AdcReader* adc_reader,
                                     unsigned int adc_channel)
    : adc_reader_(adc_reader), adc_channel_(adc_channel) {
  assert(adc_reader_->IsChannelAvailable(adc_channel_));
}
bool CaSignalProcessor::GetCurrentCaSignalLevel() {
  if (enough_samples_ == 0) {
    return false;
  }
  const uint32_t mean = samples_sum_ / kBufferCapacity;
  return mean > kAdcThresholdForLevelChange;
}

void CaSignalProcessor::PeriodicCallback() {
  const uint16_t adc_max_val = 4095;
  /* clamp to adc_max_val just in case */
  const uint16_t adc_raw =
      std::min(adc_reader_->GetRawAdcValue(adc_channel_), adc_max_val);
  /* so that 4095 is when someone is calling, 0 when not */
  const uint16_t signal_sample = adc_max_val - adc_raw;
  const bool buffer_full = samples_.IsFull();
  if (!buffer_full) {
    samples_.Push(signal_sample);
    samples_sum_ += signal_sample;
    return;
  }
  uint16_t oldest_sample;
  samples_.Peek(oldest_sample);
  samples_.Push(signal_sample);
  samples_sum_ += signal_sample;
  /* TODO: log in case of fail */
  assert(samples_sum_ >= oldest_sample);
  samples_sum_ -= oldest_sample;
  enough_samples_ = 1;
}
