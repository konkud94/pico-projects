#pragma once
#include <inttypes.h>

#include "../interfaces/relay_controller.hpp"

class RelayControllerImpl : public RelayController {
 public:
  RelayControllerImpl(const unsigned int pin);
  void Activate() override;
  void Deactivate() override;
  bool IsActivated() const override;

 private:
  const unsigned int pin_;
  bool activated_ = false;
};