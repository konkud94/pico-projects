#pragma once

#include "../interfaces/ca_signal_processor.hpp"
#include "../interfaces/clock.h"
#include "../interfaces/consent_provider.hpp"
#include "../interfaces/electric_lock.hpp"
#include "../interfaces/time_aware.h"

class CaSignalObserver : public TimeAware {
 public:
  CaSignalObserver(Clock* clock, uint32_t interval_ms,
                   CaSignalProcessor* ca_signal_processor,
                   ConsentProvider* consent_provider, ElectricLock* door_lock);

 protected:
  void InternalUpdate(const uint64_t current_ms) override;
  void OnCaSignalChange(const bool current_state);

  CaSignalProcessor* const ca_signal_processor_;
  ConsentProvider* const consent_provider_;
  ElectricLock* const door_lock_;

  bool ca_signal_last_value_ = false;
};