#pragma once

#include "../interfaces/consent_provider.hpp"

class ConsentProviderImpl : public ConsentProvider {
 public:
  ConsentProviderImpl() = default;
  bool GetDoorOpenConsent() override;
};