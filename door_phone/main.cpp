#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include "initializer_list"
#include "hardware/adc.h"



struct AdcReadRecord{
    AdcReadRecord(uint16_t raw, float v) : raw_val(raw), voltage_v(v){}
    const uint16_t raw_val;
    const float voltage_v;
};
struct OutputPin{
    OutputPin(const unsigned int pin, const char* const n) : hardware_pin(pin), name(n){}
    const unsigned int hardware_pin;
    const char* const name;
};

uint32_t GetMsSinceReboot(){
    const auto absolute = get_absolute_time();
    return to_ms_since_boot(absolute);
}
AdcReadRecord GetAdcReadings(){
    // 12-bit conversion, assume max value == ADC_VREF == 3.3 V
    static const float conversion_factor = 3.3f / (1 << 12);
    const uint16_t result = adc_read();
    return AdcReadRecord(result, result * conversion_factor);

}
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

    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    // Select ADC input 2 (GPIO28)
    adc_gpio_init(28);
    adc_select_input(2);
    sleep_ms(10'000);
    printf("timeStampMS, adcRawVal, AdcVoltage\n");
    const unsigned int sample_rate_ms = 5;
    while (1) {
        const uint32_t before_ms = GetMsSinceReboot();
        const auto adc_readings = GetAdcReadings();
        const uint32_t after_ms = GetMsSinceReboot();
        const uint32_t took_ms = after_ms - before_ms;
        printf("%u, %u, %.3f\n", after_ms, adc_readings.raw_val, adc_readings.voltage_v);
        if(took_ms < sample_rate_ms){
            sleep_ms(sample_rate_ms - took_ms);
        }
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
