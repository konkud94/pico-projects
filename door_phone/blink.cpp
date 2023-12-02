#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include "initializer_list"


// uint32_t GetMsSinceReboot(){
//     const auto absolute = get_absolute_time();
//     return to_ms_since_boot(absolute);
// }

struct OutputPin{
    OutputPin(const unsigned int pin, const char* const n) : hardware_pin(pin), name(n){}
    const unsigned int hardware_pin;
    const char* const name;
};
int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }
    OutputPin out_pins[2] = {
        OutputPin(0, "lock_pin"),
        OutputPin(1, "ca_pin"),
    };
    for(const auto &pin : out_pins){
        gpio_init(pin.hardware_pin);
        gpio_set_dir(pin.hardware_pin, GPIO_OUT);
        gpio_put(pin.hardware_pin, true);
    }

    while(1){
        const auto& out_pin = out_pins[1];
        const auto hardware_pin = out_pin.hardware_pin;
        {
            const bool prev_pin_val = gpio_get_out_level(hardware_pin);
            gpio_put(hardware_pin, !prev_pin_val);
        }
        const bool current_pin_val = gpio_get_out_level(hardware_pin);
        const char* const lock_state = current_pin_val? "DEACTIVATED" : "ACTIVATED";
        printf("Current ELock state is: %s\n", lock_state);
        sleep_ms(8*1000);
    }
    while (true) {
        static constexpr uint32_t delayMs = 1000;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(delayMs);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(delayMs);
    }
}
