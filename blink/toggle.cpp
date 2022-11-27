#include "pico/stdlib.h"
#include "toggle.hh"

CPinToggler::CPinToggler(const unsigned int pin) : m_pin(pin)
{
    gpio_init(m_pin);
    gpio_set_dir(m_pin, GPIO_OUT);
    gpio_put(m_pin, false);
}
void CPinToggler::TogglePin() const
{
    gpio_put(m_pin, m_pinState);
    m_pinState = !m_pinState;
}
