
#include "ca_signal_observer.hpp"

CaSignalObserver::CaSignalObserver(Clock* clock, uint32_t interval_ms,
                                   CaSignalProcessor* ca_signal_processor,
                                   ConsentProvider* consent_provider)
    : TimeAware(clock, interval_ms),
      ca_signal_processor_(ca_signal_processor),
      consent_provider_(consent_provider) {}

void CaSignalObserver::InternalUpdate(const uint64_t current_ms) {
  (void)current_ms;

  const bool ca_signal_value = ca_signal_processor_->GetCurrentCaSignalLevel();
  if (ca_signal_last_value_ != ca_signal_value) {
    ca_signal_last_value_ = ca_signal_value;
    OnCaSignalChange(ca_signal_value);
  }
}

void CaSignalObserver::OnCaSignalChange(const bool current_state) {
  /* TODO: log signal change along with current_state */
  if (current_state && consent_provider_->GetDoorOpenConsent()) {
    /*
      TODO: finished here, open door
      lock_ctrl -> OpenDoor();
    */
  }
}
