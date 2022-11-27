/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "toggle.hh"

int main() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint32_t intervalMS = 1000;
    const CPinToggler pT(PICO_DEFAULT_LED_PIN);
    while (true) {
        pT.TogglePin();
        sleep_ms(intervalMS);
    }
#endif
}
