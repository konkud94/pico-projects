#include "door_electric_lock.hpp"

DoorElectricLock::DoorElectricLock(Clock* clock, uint32_t interval_ms,
                                   RelayController* door_relay,
                                   uint32_t wait_before_open_ms)
    : TimeAware(clock, interval_ms),
      clock_(clock),
      door_relay_(door_relay),
      wait_before_open_ms_(wait_before_open_ms) {}

void DoorElectricLock::InternalUpdate(const uint64_t current_ms) {
  if (current_state_ == State::IDLE) {
    return;
  }
  if (current_state_ == State::WAIT_BEFORE_OPEN) {
    const bool time_passed = current_ms >= time_stamp_wait_until_;
    if (!time_passed) {
      return;
    }
    door_relay_->Activate();
    current_state_ = State::LOCK_ON;
    return;
  }
  if (current_state_ == State::LOCK_ON) {
    const bool time_passed = current_ms >= time_stamp_lock_on_until_;
    if (!time_passed) {
      return;
    }
    door_relay_->Deactivate();
    current_state_ = State::IDLE;
    return;
  }
}

void DoorElectricLock::Open(uint32_t open_for_ms) {
  if (current_state_ != State::IDLE) {
    return;
  }
  current_state_ = State::WAIT_BEFORE_OPEN;
  time_stamp_wait_until_ = clock_->GetMsSinceBoot() + wait_before_open_ms_;
  time_stamp_lock_on_until_ =
      clock_->GetMsSinceBoot() + wait_before_open_ms_ + open_for_ms;
}