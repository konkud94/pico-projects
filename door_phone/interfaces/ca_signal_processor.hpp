#pragma once
#include <inttypes.h>

class CaSignalProcessor {
 public:
  virtual ~CaSignalProcessor() = default;
  virtual bool GetCurrentCaSignalLevel() = 0;
};