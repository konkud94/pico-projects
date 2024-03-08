#pragma once

class ConsentProvider {
 public:
  virtual ~ConsentProvider() = default;
  virtual bool GetDoorOpenConsent() = 0;
};