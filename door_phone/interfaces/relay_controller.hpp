#pragma once
#include <inttypes.h>

class RelayController {
 public:
  virtual ~RelayController() = default;
  virtual void Activate() = 0;
  virtual void Deactivate() = 0;
  virtual bool IsActivated() const = 0;
};