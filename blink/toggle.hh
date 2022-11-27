#pragma once

class CPinToggler
{
public:
    CPinToggler(const unsigned int pin);
    ~CPinToggler() = default;
    void TogglePin() const;
private:
    const unsigned int m_pin;
    mutable bool m_pinState = false;
};