#pragma once

#include <inttypes.h>
#include <stdlib.h>

#include "../algorithms_and_structures/circular_buffer.h"
#include "../interfaces/adc_reader.h"

class CaSignalProcessor {
  static constexpr size_t kBufferCapacity = 50;
  static constexpr uint16_t kAdcThresholdForLevelChange = 1000;

 public:
  CaSignalProcessor(AdcReader* adc_reader, unsigned int adc_channel);
  /* true - on */
  bool GetCurrentCaSignalLevel();
  /* to be called from ISR */
  void PeriodicCallback();

 private:
  AdcReader* const adc_reader_;
  const unsigned int adc_channel_;
  CurcularBuffer<uint16_t, kBufferCapacity> samples_;
  uint32_t samples_sum_ = 0;
  uint32_t enough_samples_ = 0;
};