#pragma once

#include "../interfaces/clock.h"
#include "../interfaces/electric_lock.hpp"
#include "../interfaces/relay_controller.hpp"
#include "../interfaces/time_aware.h"

class DoorElectricLock : public TimeAware, public ElectricLock {
  enum class State {
    IDLE = 0,
    WAIT_BEFORE_OPEN = 1,
    LOCK_ON = 2,
  };

 public:
  DoorElectricLock(Clock* clock, uint32_t interval_ms,
                   RelayController* door_relay, uint32_t wait_before_open_ms);
  void Open(uint32_t open_for_ms) override;

 protected:
  void InternalUpdate(const uint64_t current_ms) override;
  Clock* const clock_;
  RelayController* const door_relay_;
  const uint32_t wait_before_open_ms_;

  State current_state_ = State::IDLE;
  uint64_t time_stamp_wait_until_;
  uint64_t time_stamp_lock_on_until_;
};