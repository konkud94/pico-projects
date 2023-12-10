#pragma once
#include <inttypes.h>
class AdcReader {
 public:
  virtual ~AdcReader() = default;
  virtual bool IsChannelAvailable(unsigned int channel) const = 0;
  /* should return 0 for uninitialized channel */
  virtual uint16_t GetRawAdcValue(unsigned int channel) const = 0;
};